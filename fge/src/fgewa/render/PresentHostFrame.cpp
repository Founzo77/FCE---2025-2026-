#include <fgewa/render/PresentHostFrame.hpp>
#include <fgewa/render/data/Device.hpp>
#include <fgewa/render/data/Frame.hpp>

#include <fge/render/data/BarrierBatch.hpp>
#include <fge/utility.hpp>

#include <d3dx12.h>
#include <anari/anari.h>

#include <algorithm>

using fge::throwIfFailed;

namespace fgewa
{
    void PresentHostFrame::initialize(shared_ptr<Device> device, 
            HWND hWnd, ComPtr<ID3D12Device5> dxrDevice, ID3D12CommandQueue* commandQueue,
            const uint32_t width, const uint32_t height, const uint32_t nbBackBuffers)
    {
        PresentFrame::initialize(
            device, hWnd, dxrDevice, commandQueue, width, height, nbBackBuffers);

        D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};
        UINT numRows = 0;
        UINT64 rowSizeInBytes = 0;
        UINT64 totalBytes = 0;
        m_dxrDevice->GetCopyableFootprints(
            &m_bufferTargetDescription, 0, 1, 0, &footprint, &numRows, &rowSizeInBytes, &totalBytes);
        m_anaryUploadBuffer.initialize(m_dxrDevice, totalBytes);
    }

    void PresentHostFrame::copyOutputToRTV(ID3D12GraphicsCommandList4* commandList, Frame& frame)
    {
        // Map ANARI
        uint32_t width=0, height=0; ANARIDataType type=ANARI_UNKNOWN;
        const void* mapped = anariMapFrame(m_device->getHandle(), frame.getHandle(), 
            "channel.color", &width, &height, &type);
        if(mapped == nullptr)
            return;
        throwIfFailed(mapped != nullptr, "Mapped Frame failed");

        // Préparer footprint pour CopyTextureRegion
        D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};
        UINT numRows = 0;
        UINT64 rowSizeInBytes = 0;
        UINT64 totalBytes = 0;

        m_dxrDevice->GetCopyableFootprints(
            &m_bufferTargetDescription, 0, 1, 0, &footprint, &numRows, &rowSizeInBytes, &totalBytes);

        // TO_DO verifier la taille du buffer en cas de resize
        uint8_t* dst = static_cast<uint8_t*>(m_anaryUploadBuffer.startUpload());
        CD3DX12_RANGE r(0,0);

        uint8_t* dstPixels = dst + footprint.Offset;
        const UINT dstPitch = footprint.Footprint.RowPitch;

        for (uint32_t y = 0; y < height; ++y)
        {
            uint8_t* row = dstPixels + y * dstPitch;
            uint32_t flippedY = height - 1 - y;

            for (uint32_t x = 0; x < width; ++x)
            {
                uint32_t flippedX = width - 1 - x;
                uint32_t i = flippedY * width + flippedX;

                uint8_t R, G, B, A=255;

                if (type == ANARI_FLOAT32_VEC4)
                {
                    const float* p = (const float*)mapped;
                    R = (uint8_t)(std::clamp(p[i*4+0],0.f,1.f)*255.f);
                    G = (uint8_t)(std::clamp(p[i*4+1],0.f,1.f)*255.f);
                    B = (uint8_t)(std::clamp(p[i*4+2],0.f,1.f)*255.f);
                }
                else if (type == ANARI_UFIXED8_VEC4)
                {
                    const uint8_t* p = (const uint8_t*)mapped;
                    R = p[i*4+0]; G = p[i*4+1]; B = p[i*4+2]; A = p[i*4+3];
                }
                else throwIfFailed(false, "Unsupported pixelType");

                row[x*4+0] = R;
                row[x*4+1] = G;
                row[x*4+2] = B;
                row[x*4+3] = A;
            }
        }

        m_anaryUploadBuffer.endUpload();
        anariUnmapFrame(m_device->getHandle(), frame.getHandle(), "channel.color");

        D3D12_TEXTURE_COPY_LOCATION src{};
        src.pResource = m_anaryUploadBuffer.getBuffer().Get();
        src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
        src.PlacedFootprint = footprint;

        D3D12_TEXTURE_COPY_LOCATION copyDestination{};
        copyDestination.pResource = 
            m_framePresentationContexts[m_currentIdBackBuffer].m_backBuffer.Get();
        copyDestination.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        copyDestination.SubresourceIndex = 0;

        commandList->CopyTextureRegion(&copyDestination, 0, 0, 0, &src, nullptr);
    }
}