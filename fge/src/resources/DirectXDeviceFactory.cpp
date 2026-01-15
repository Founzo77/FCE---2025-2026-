#include <fge/resources/DirectXDeviceFactory.hpp>
#include <fge/utility.hpp>

#include <d3d11.h>
#include <d3d11on12.h>
#include <d2d1_1.h>
#include <d2d1_3.h>

#include <stdexcept>

namespace fge
{
    ComPtr<ID3D12Device5> DirectXDeviceFactory::buildDirectXDeviceV5(
        ComPtr<IDXGIAdapter4> adapter)
    {
        ComPtr<ID3D12Device5> device5;

        throwIfFailed(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_2, 
            IID_PPV_ARGS(&device5)), "Failed to create ID3D12Device5");

        #if defined(_DEBUG)
            ComPtr<ID3D12InfoQueue> infoQueue;
            if (SUCCEEDED(device5.As(&infoQueue)))
            {
                infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
                infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
                //infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

                // Suppress messages based on their severity level
                D3D12_MESSAGE_SEVERITY severities[] =
                {
                    D3D12_MESSAGE_SEVERITY_INFO
                };
        
                // Suppress individual messages by their ID
                D3D12_MESSAGE_ID denyIds[] = {
                    // I'm really not sure how to avoid this message.
                    D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
                    // This warning occurs when using capture frame while graphics debugging.
                    D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
                    // This warning occurs when using capture frame while graphics debugging.             
                    D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
                };
        
                D3D12_INFO_QUEUE_FILTER newFilters = {};
                //newFilters.DenyList.NumCategories = _countof(Categories);
                //newFilters.DenyList.pCategoryList = Categories;
                newFilters.DenyList.NumSeverities = _countof(severities);
                newFilters.DenyList.pSeverityList = severities;
                newFilters.DenyList.NumIDs = _countof(denyIds);
                newFilters.DenyList.pIDList = denyIds;
        
                throwIfFailed(infoQueue->PushStorageFilter(&newFilters),
                    "Failed to push filters");
            }
        #endif

        return device5;
    }

    void DirectXDeviceFactory::buildDirectX11On12Device(ComPtr<ID3D12Device> device,
        ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D11Device>* device11, 
        ComPtr<ID3D11DeviceContext>* device11Context, ComPtr<ID3D11On12Device2>* device11On12)
    {
        UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
        #if defined(_DEBUG)
            //deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
        #endif

        ComPtr<ID3D11Device> tempDevice11;
        ComPtr<ID3D11DeviceContext> tempDevice11Context;


        throwIfFailed(D3D11On12CreateDevice(device.Get(), deviceFlags,
                nullptr, 0, // Pas de feature levels spécifiques
                reinterpret_cast<IUnknown**>(commandQueue.GetAddressOf()), 1,
                0, // NodeMask = 0 (single GPU)
                &tempDevice11, &tempDevice11Context,
                nullptr // Pas besoin de feature level out
            ), "Failed to create D3D11On12 device");

        ComPtr<ID3D11On12Device2> tempDevice11On12;
        throwIfFailed(tempDevice11.As(&tempDevice11On12), "Failed to query ID3D11On12Device2");

        *device11 = tempDevice11;
        *device11Context = tempDevice11Context;
        *device11On12 = tempDevice11On12;
    }

    ComPtr<IDXGIDevice> DirectXDeviceFactory::buildDxgiDevice(ComPtr<ID3D11On12Device2> device11On12)
    {
        ComPtr<IDXGIDevice> dxgiDevice;
        throwIfFailed(device11On12.As(&dxgiDevice), 
            "Failed to query IDXGIDevice from ID3D11On12Device2");
        return dxgiDevice;
    }

    ComPtr<ID2D1Device> DirectXDeviceFactory::build2DDevice(ComPtr<IDXGIDevice> dxgiDevice)
    {
        D2D1_FACTORY_OPTIONS options = {};
        #if defined(_DEBUG)
            //options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
        #endif

        ComPtr<ID2D1Factory1> d2dFactory;
        throwIfFailed(
            D2D1CreateFactory(
                D2D1_FACTORY_TYPE_SINGLE_THREADED,
                __uuidof(ID2D1Factory1),
                &options,
                &d2dFactory
            ),
            "Failed to create D2D1Factory3"
        );

        ComPtr<ID2D1Device> device2D;
        throwIfFailed(d2dFactory->CreateDevice(dxgiDevice.Get(), &device2D),
            "Failed to create ID2D1Device");

        return device2D;
    }

    ComPtr<ID2D1DeviceContext> DirectXDeviceFactory::buildDirectX2DDeviceContext(
        ComPtr<ID2D1Device> device2D)
    {
        ComPtr<ID2D1DeviceContext> d2dContext;
        throwIfFailed(device2D->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dContext),
            "Failed to create ID2D1DeviceContext");

        return d2dContext;
    }
}