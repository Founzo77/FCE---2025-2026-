#pragma once

#include <Windows.h>
#include <wrl/client.h>
#include <d3d12.h>

#include <cstdint>
#include <vector>
#include <string>

using namespace Microsoft::WRL;

using std::vector;
using std::string;

namespace fge
{
    class ShaderFactory
    {
    public:
        static vector<char> loadShader(const string& filePath);
        
    };
}
