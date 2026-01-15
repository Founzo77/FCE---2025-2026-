#include <fge/render/WriterFrame.hpp>

#include <fge/render/FrameContext.hpp>

#include <fge/utility.hpp>

namespace fge
{
    void WriterFrame::initialize(ComPtr<ID3D11On12Device2> device11On12, 
        ComPtr<ID2D1DeviceContext> device2DContext, vector<FrameContext>& framesTarget)
    {
        m_11On12Device = device11On12;
        m_2DDeviceContext = device2DContext;

        D3D11_RESOURCE_FLAGS resourceFlag{D3D11_BIND_RENDER_TARGET};

        for(int i = 0; i < framesTarget.size(); i++)
        {
            ComPtr<ID3D11Resource> wrappedResource;
            throwIfFailed(m_11On12Device->CreateWrappedResource(
                framesTarget[i].m_backBuffer.Get(), &resourceFlag,
                D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PRESENT,
                IID_PPV_ARGS(&wrappedResource)), "Failed to create wrapped resource");
            
            ComPtr<IDXGISurface> surface;
            wrappedResource.As(&surface);
            
            // TO_DO Faire les dpi
            //float dpi = GetDpiForWindow(Win32Application::GetHwnd());

            D2D1_BITMAP_PROPERTIES1 bitmapProperties =
                D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
                D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
            
            ComPtr<ID2D1Bitmap1> renderTarget;
            throwIfFailed(m_2DDeviceContext->CreateBitmapFromDxgiSurface(surface.Get(),
                &bitmapProperties, &renderTarget), "Failed to create bitmap from Dxgi surface");

            m_11WrappedResources.push_back(wrappedResource);
            m_surfaces.push_back(surface);
            m_2DRenderTargets.push_back(renderTarget);
        }

        ComPtr<IDWriteFactory> dwriteFactory;
        throwIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(dwriteFactory.GetAddressOf())), 
            "Failed to create DWrite factory");
        throwIfFailed(dwriteFactory->CreateTextFormat(L"Consolas", // police
            nullptr,
            DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            20.0f, // taille
            L"en-us", &m_textFormat), "Failed to create text format");

        throwIfFailed(m_2DDeviceContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Yellow), &m_brush), "Failed to create solid color brush");
    }

    void WriterFrame::writeToTexture(const string& text, UINT renderTargetIndex)
    {
        // Indique qu’on va dessiner sur la ressource DXR
        m_11On12Device->AcquireWrappedResources(
            m_11WrappedResources[renderTargetIndex].GetAddressOf(), 1);

        m_2DDeviceContext->SetTarget(m_2DRenderTargets[renderTargetIndex].Get());
        m_2DDeviceContext->BeginDraw();
        
        m_2DDeviceContext->DrawTextA(
            L"Hello DXR World!",
            16,
            m_textFormat.Get(),
            D2D1::RectF(10, 10, 500, 200),
            m_brush.Get()
        );

        m_2DDeviceContext->EndDraw();

        // Libère la ressource pour que DXR puisse l'utiliser à nouveau
        m_11On12Device->ReleaseWrappedResources(
            m_11WrappedResources[renderTargetIndex].GetAddressOf(), 1);
    }
}