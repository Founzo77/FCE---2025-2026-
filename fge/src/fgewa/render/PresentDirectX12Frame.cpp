#include <fgewa/render/PresentDirectX12Frame.hpp>
#include <fgewa/render/data/Device.hpp>
#include <fgewa/render/data/Frame.hpp>

#include <fge/utility.hpp>

#include <d3dx12.h>
#include <anari/anari.h>

using fge::throwIfFailed;
using Microsoft::WRL::ComPtr;

namespace fgewa
{
    PresentDirectX12Frame::~PresentDirectX12Frame()
    {
        m_sharedTexture.Reset();
    }

    void PresentDirectX12Frame::initialize(shared_ptr<Device> device,
        HWND hWnd, ComPtr<ID3D12Device5> dxrDevice, ID3D12CommandQueue* commandQueue,
        const uint32_t width, const uint32_t height, const uint32_t nbBackBuffers)
    {
        PresentFrame::initialize(device, hWnd, dxrDevice, commandQueue, width, height, nbBackBuffers);

        // rien d'autre à initialiser : la ressource sera ouverte via HANDLE à chaque frame
    }

    void PresentDirectX12Frame::copyOutputToRTV(ID3D12GraphicsCommandList4* commandList,
        Frame& frame)
    {
        uint32_t width = 0;
        uint32_t height = 0;
        ANARIDataType type = ANARI_UNKNOWN;

        // récupérer HANDLE DX12
        HANDLE sharedHandle = (HANDLE)anariMapFrame(
            m_device->getHandle(), frame.getHandle(), "channel.dx12Color",
            &width, &height, &type);

        if (!sharedHandle)
            return;

        throwIfFailed(type == ANARI_VOID_POINTER, "channel.dx12Color must return ANARI_VOID_POINTER");
        if(m_sharedTexture == nullptr)
            throwIfFailed(m_dxrDevice->OpenSharedHandle(sharedHandle, IID_PPV_ARGS(&m_sharedTexture)), 
                "Failed to create shared output resource");

        ID3D12Resource* backBuffer = 
            m_framePresentationContexts[m_currentIdBackBuffer].m_backBuffer.Get();

        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_sharedTexture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COPY_SOURCE);

        commandList->ResourceBarrier(1, &barrier);
        commandList->CopyResource(backBuffer, m_sharedTexture.Get());

        std::swap(barrier.Transition.StateBefore, barrier.Transition.StateAfter);
        commandList->ResourceBarrier(1, &barrier);

        anariUnmapFrame(m_device->getHandle(), frame.getHandle(), "channel.dx12Color");
    }
}