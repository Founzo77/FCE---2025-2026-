#pragma once

#include "PresentFrame.hpp"

#include <d3d12.h>
#include <wrl.h>
#include <cuda.h>
#include <cuda_runtime.h>

namespace fgewa
{
    class PresentCudaFrame : public PresentFrame
    {
    private:
        // D3D12 staging buffer (GPU)
        Microsoft::WRL::ComPtr<ID3D12Resource> m_interopBuffer;

        // Footprint for CopyTextureRegion
        D3D12_PLACED_SUBRESOURCE_FOOTPRINT m_footprint{};
        UINT   m_numRows{0};
        UINT64 m_rowSize{0};
        UINT64 m_totalBytes{0};

        // CUDA external memory
        cudaExternalMemory_t m_externalMemory{nullptr};
        void* m_cudaMappedPtr{nullptr}; // device ptr mapped to D3D12 resource
        size_t m_cudaMappedSize{0};

        bool m_ready{false};

    public:
        PresentCudaFrame() = default;
        ~PresentCudaFrame() override;

        void initialize(shared_ptr<Device> device, 
            HWND hWnd, ComPtr<ID3D12Device5> dxrDevice, ID3D12CommandQueue* commandQueue,
            const uint32_t width, const uint32_t height, const uint32_t nbBackBuffers) override;

        void copyOutputToRTV(ID3D12GraphicsCommandList4* commandList, Frame& frame) override;

        void copyWithFlipYAxis(uchar4* d_dst, size_t dstPitchBytes,
            const uchar4* d_src, size_t srcPitchBytes, int width, int height);
    };
}