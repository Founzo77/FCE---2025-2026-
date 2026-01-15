#include <fge/resources/AdapterFactory.hpp>
#include <fge/utility.hpp>

#include <d3dcommon.h>
#include <d3d12.h>

namespace fge
{
    ComPtr<IDXGIAdapter4> AdapterFactory::buildAdapterV4()
    {
        ComPtr<IDXGIFactory4> factory;
        UINT createFactoryFlags = 0;

        #if defined(_DBEBUG)
            createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
        #endif

        throwIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&factory)),
            "Failed to create DXGIFactory2");
        
        ComPtr<IDXGIAdapter1> adapter1;
        ComPtr<IDXGIAdapter4> adapter4;
        SIZE_T maxDedicatedVideoMemory = 0;

        for (UINT i = 0; factory->EnumAdapters1(i, &adapter1) != 
            DXGI_ERROR_NOT_FOUND; i++)
        {
            DXGI_ADAPTER_DESC1 adapterDesc;
            adapter1->GetDesc1(&adapterDesc);
 
            // Check to see if the adapter can create a D3D12 device without actually 
            // creating it. The adapter with the largest dedicated video memory
            // is favored.
            if ((adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
                SUCCEEDED(D3D12CreateDevice(adapter1.Get(), 
                    D3D_FEATURE_LEVEL_12_2, __uuidof(ID3D12Device), nullptr)) && 
                adapterDesc.DedicatedVideoMemory > maxDedicatedVideoMemory )
            {
                maxDedicatedVideoMemory = adapterDesc.DedicatedVideoMemory;
                throwIfFailed(adapter1.As(&adapter4), 
                    "Failed to convert IDXGIAdapter1 to IDXGIAdapter4");
            }
        }
        
        return adapter4;
    }
} // namespace fge
