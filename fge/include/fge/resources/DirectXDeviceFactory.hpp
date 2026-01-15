#pragma once

#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>

using namespace Microsoft::WRL;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11On12Device2;
struct ID2D1Device;
struct ID2D1DeviceContext;

namespace fge
{
    class DirectXDeviceFactory
    {
    public:
        static ComPtr<ID3D12Device5> buildDirectXDeviceV5(ComPtr<IDXGIAdapter4> adapter);

        static void buildDirectX11On12Device(ComPtr<ID3D12Device> device,
            ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D11Device>* device11, 
            ComPtr<ID3D11DeviceContext>* device11Context, ComPtr<ID3D11On12Device2>* device11On12);

        static ComPtr<IDXGIDevice> buildDxgiDevice(ComPtr<ID3D11On12Device2> device11On12);

        static ComPtr<ID2D1Device> build2DDevice(ComPtr<IDXGIDevice> dxgiDevice);

        static ComPtr<ID2D1DeviceContext> buildDirectX2DDeviceContext(ComPtr<ID2D1Device> device2D);
    };
}
