#pragma once

#include <dxgi1_6.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;

namespace fge
{
    class AdapterFactory
    {
    public:
        static ComPtr<IDXGIAdapter4> buildAdapterV4();
    };
}
