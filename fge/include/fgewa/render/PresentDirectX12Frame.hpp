#pragma once

#include "PresentFrame.hpp"

#include <fge/render/memory/UploadBuffer.hpp>

namespace fgewa
{
    class PresentDirectX12Frame : public PresentFrame
    {
    private:
        ComPtr<ID3D12Resource> m_sharedTexture;

    public:
        PresentDirectX12Frame() = default;
        ~PresentDirectX12Frame() override;

        PresentDirectX12Frame(const PresentDirectX12Frame&) = default;
        PresentDirectX12Frame& operator=(const PresentDirectX12Frame&) = default;
        PresentDirectX12Frame(PresentDirectX12Frame&& other) = default;
        PresentDirectX12Frame& operator=(PresentDirectX12Frame&& other) = default;

        virtual void initialize(shared_ptr<Device> device, 
            HWND hWnd, ComPtr<ID3D12Device5> dxrDevice, ID3D12CommandQueue* commandQueue,
            const uint32_t width, const uint32_t height, const uint32_t nbBackBuffers) override;
        virtual void copyOutputToRTV(ID3D12GraphicsCommandList4* commandList, Frame& frame) override;
    };
}