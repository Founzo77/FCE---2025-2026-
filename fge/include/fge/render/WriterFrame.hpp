#pragma once

#include <Windows.h>
#include <wrl/client.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>

#include <d3d11.h>
#include <d3d11on12.h>
#include <d2d1_1.h>
#include <dwrite.h>

#include <string>
#include <vector>

using namespace Microsoft::WRL;
using std::string;
using std::vector;

namespace fge
{
    class FrameContext;

    class WriterFrame
    {
    private:
        ComPtr<ID3D11On12Device2> m_11On12Device;
        ComPtr<ID2D1DeviceContext> m_2DDeviceContext;
        vector<ComPtr<ID3D11Resource>> m_11WrappedResources;
        vector<ComPtr<IDXGISurface>> m_surfaces;
        vector<ComPtr<ID2D1Bitmap1>> m_2DRenderTargets;
        ComPtr<IDWriteTextFormat> m_textFormat;
        ComPtr<ID2D1SolidColorBrush> m_brush;

    public:
        WriterFrame() = default;
        ~WriterFrame() = default;

        void initialize(ComPtr<ID3D11On12Device2> device11On12, 
            ComPtr<ID2D1DeviceContext> device2DContext, vector<FrameContext>& framesTarget);
        void writeToTexture(const string& text, UINT renderTargetIndex);

        inline ComPtr<ID3D11Resource> get11WrappedResource(UINT index)
        {
            return m_11WrappedResources[index];
        }
    };
}
