#pragma once

#include "PresentFrame.hpp"

#include <fge/render/memory/UploadBuffer.hpp>

namespace fgewa
{
    class PresentHostFrame : public PresentFrame
    {
    private:
        fge::UploadBuffer m_anaryUploadBuffer;

    public:
        PresentHostFrame() = default;
        virtual ~PresentHostFrame() = default;

        PresentHostFrame(const PresentHostFrame&) = default;
        PresentHostFrame& operator=(const PresentHostFrame&) = default;
        PresentHostFrame(PresentHostFrame&& other) = default;
        PresentHostFrame& operator=(PresentHostFrame&& other) = default;

        virtual void initialize(shared_ptr<Device> device, 
            HWND hWnd, ComPtr<ID3D12Device5> dxrDevice, ID3D12CommandQueue* commandQueue,
            const uint32_t width, const uint32_t height, const uint32_t nbBackBuffers) override;
        virtual void copyOutputToRTV(ID3D12GraphicsCommandList4* commandList, Frame& frame) override;
    };
}