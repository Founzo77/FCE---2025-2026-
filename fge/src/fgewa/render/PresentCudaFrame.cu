#include <fgewa/render/PresentCudaFrame.hpp>
#include <fgewa/render/data/Device.hpp>
#include <fgewa/render/data/Frame.hpp>

#include <fge/utility.hpp>

#include <d3dx12.h>
#include <anari/anari.h>

#include <windows.h>

using fge::throwIfFailed;
using Microsoft::WRL::ComPtr;

namespace fgewa
{
    static void throwIfCudaFailed(cudaError_t err, const char* msg)
    {
        throwIfFailed(err == cudaSuccess, msg);
    }

    PresentCudaFrame::~PresentCudaFrame()
    {
        // Release CUDA external memory
        if (m_externalMemory) 
        {
            cudaDestroyExternalMemory(m_externalMemory);
            m_externalMemory = nullptr;
        }
        m_cudaMappedPtr = nullptr;
        m_cudaMappedSize = 0;
        m_ready = false;
    }

    void PresentCudaFrame::initialize(shared_ptr<Device> device, 
        HWND hWnd, ComPtr<ID3D12Device5> dxrDevice, ID3D12CommandQueue* commandQueue,
        const uint32_t width, const uint32_t height, const uint32_t nbBackBuffers)
    {
        PresentFrame::initialize(
            device, hWnd, dxrDevice, commandQueue, width, height, nbBackBuffers);

        // Compute footprint (layout of backbuffer copy)
        m_dxrDevice->GetCopyableFootprints(&m_bufferTargetDescription,
            0, 1, 0, &m_footprint, &m_numRows, &m_rowSize, &m_totalBytes);

        // Create D3D12 buffer shareable with CUDA
        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
        CD3DX12_RESOURCE_DESC bufDesc = CD3DX12_RESOURCE_DESC::Buffer(m_totalBytes);

        throwIfFailed(m_dxrDevice->CreateCommittedResource(&heapProps,
            D3D12_HEAP_FLAG_SHARED, // IMPORTANT
            &bufDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&m_interopBuffer)));

        // Create shared Win32 handle for the resource
        HANDLE sharedHandle = nullptr;
        throwIfFailed(m_dxrDevice->CreateSharedHandle(m_interopBuffer.Get(),
            nullptr, // SECURITY_ATTRIBUTES (can be null)
            GENERIC_ALL, nullptr, &sharedHandle));

        // Import as CUDA external memory
        D3D12_RESOURCE_ALLOCATION_INFO allocInfo =
        m_dxrDevice->GetResourceAllocationInfo(0, 1, &bufDesc);

        cudaExternalMemoryHandleDesc memDesc{};
        memDesc.type = cudaExternalMemoryHandleTypeD3D12Resource;
        memDesc.handle.win32.handle = sharedHandle;
        memDesc.size = allocInfo.SizeInBytes;  // IMPORTANT: allocation size, not just logical size
        memDesc.flags = cudaExternalMemoryDedicated;

        throwIfCudaFailed(cudaImportExternalMemory(&m_externalMemory, &memDesc),
            "cudaImportExternalMemory failed");

        CloseHandle(sharedHandle);

        // Map CUDA pointer to that memory
        cudaExternalMemoryBufferDesc bufMap{};
        bufMap.offset = 0;
        bufMap.size = (size_t)m_totalBytes;
        bufMap.flags = 0;

        throwIfCudaFailed(cudaExternalMemoryGetMappedBuffer(&m_cudaMappedPtr, 
            m_externalMemory, &bufMap), "cudaExternalMemoryGetMappedBuffer failed");

        m_cudaMappedSize = (size_t)m_totalBytes;
        m_ready = true;
    }

    void PresentCudaFrame::copyOutputToRTV(ID3D12GraphicsCommandList4* commandList, Frame& frame)
    {
        throwIfFailed(m_ready, "CUDA presenter not initialized");

        // Map ANARI CUDA buffer (VisRTX "colorCUDA")
        uint32_t width = 0, height = 0;
        ANARIDataType type = ANARI_UNKNOWN;

        const void* srcCudaPtr = anariMapFrame(m_device->getHandle(),
            frame.getHandle(), "channel.colorCUDA", &width, &height, &type);

        throwIfFailed(srcCudaPtr != nullptr, "Failed to map channel.colorCUDA");
        throwIfFailed(type == ANARI_UFIXED8_VEC4, "colorCUDA must be ANARI_UFIXED8_VEC4");

        // CUDA device->device copy into the D3D12 buffer memory
        const size_t dstPitch = (size_t)m_footprint.Footprint.RowPitch;
        const size_t srcPitch = (size_t)width * 4;

        uint8_t* dst = static_cast<uint8_t*>(m_cudaMappedPtr);
        uint8_t* dstPixels = dst + (size_t)m_footprint.Offset;

        // NOTE: No sync here yet (see below "semaphore" section)
        copyWithFlipYAxis((uchar4*)m_cudaMappedPtr, dstPitch, 
            (const uchar4*)srcCudaPtr, srcPitch, (int)width, (int)height);

        anariUnmapFrame(m_device->getHandle(), frame.getHandle(), "channel.colorCUDA");

        D3D12_TEXTURE_COPY_LOCATION src{};
        src.pResource = m_interopBuffer.Get();
        src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
        src.PlacedFootprint = m_footprint;

        D3D12_TEXTURE_COPY_LOCATION dstTexture{};
        dstTexture.pResource = 
            m_framePresentationContexts[m_currentIdBackBuffer].m_backBuffer.Get();
        dstTexture.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        dstTexture.SubresourceIndex = 0;

        commandList->CopyTextureRegion(&dstTexture, 0, 0, 0, &src, nullptr);
    }

    __global__ void blitFlipY_RGBA8(uchar4* dst, size_t dstPitchBytes,
        const uchar4* src, size_t srcPitchBytes, int width, int height)
    {
        int x = blockIdx.x * blockDim.x + threadIdx.x;
        int y = blockIdx.y * blockDim.y + threadIdx.y;

        if (x >= width || y >= height)
            return;

        int srcY = height - 1 - y;
        int srcX = width  - 1 - x;

        const uchar4* srcRow = (const uchar4*)((const char*)src + srcY * srcPitchBytes);
        uchar4* dstRow = (uchar4*)((char*)dst + y * dstPitchBytes);

        dstRow[x] = srcRow[srcX];
    }

    void PresentCudaFrame::copyWithFlipYAxis(uchar4* d_dst, size_t dstPitchBytes, 
        const uchar4* d_src, size_t srcPitchBytes, int width, int height)
    {
        const size_t dstPitch = m_footprint.Footprint.RowPitch;
        const size_t srcPitch = size_t(width) * 4;

        uchar4* d_dstPixels = (uchar4*)(d_dst + m_footprint.Offset);

        dim3 block(16, 16);
        dim3 grid((width + block.x - 1) / block.x, (height + block.y - 1) / block.y);

        blitFlipY_RGBA8<<<grid, block>>>(
            d_dstPixels, dstPitch, d_src, srcPitch, (int)width, (int)height);

        cudaDeviceSynchronize();
    }
}
