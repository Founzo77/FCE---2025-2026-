#pragma once

#include "PlatformWindows.hpp"

#include <exception>
#include <cstdint>
#include <string>
#include <sstream>
#include <d3d12.h>
#include <dxgi.h>
#include <dxgidebug.h>

namespace fge
{
    using Result = int;

    inline constexpr int SUCCESS = 0;
    inline constexpr int DEFAULT_FAIL = 1;

    inline constexpr bool isSuccess(Result result) noexcept
    {
        return result == SUCCESS;
    }

    inline constexpr Result boolToResult(bool result) noexcept
    {
        return result ? SUCCESS : DEFAULT_FAIL;
    }

    inline void throwIfFailed(HRESULT hResult)
    {
        if(FAILED(hResult))
        {
            throw std::runtime_error("Exception");
        }
    }

    inline void throwIfFailed(HRESULT hResult, const char* message)
    {
        if(FAILED(hResult))
        {
            throw std::runtime_error(message);
        }
    }

    inline void throwIfFailed(HANDLE handle)
    {
        if(!handle)
        {
            throw std::runtime_error("Exception");
        }
    }

    inline void throwIfFailed(HANDLE handle, const char* message)
    {
        if(!handle)
        {
            throw std::runtime_error(message);
        }
    }

    inline void throwIfFailed(bool handle)
    {
        if(!handle)
        {
            throw std::runtime_error("Exception");
        }
    }

    inline void throwIfFailed(bool handle, const char* message)
    {
        if(!handle)
        {
            throw std::runtime_error(message);
        }
    }

    inline void throwIfFailed(bool handle, const std::string& message)
    {
        if(!handle)
        {
            throw std::runtime_error(message.c_str());
        }
    }

    constexpr uint64_t alignData(uint64_t value, uint64_t alignment) noexcept
    {
        return (value + (alignment - 1)) & ~(alignment - 1);
    }

    constexpr uint64_t alignData256(uint64_t size) noexcept
    { 
        return (size + 255) & ~255ull; 
    }

    namespace debug
    {
        inline const char* ExtractFileName(const char* path)
        {
            const char* file = path;

            for (const char* p = path; *p; ++p)
            {
                if (*p == '/' || *p == '\\')
                    file = p + 1;
            }

            return file;
        }

        inline std::wstring BuildDebugName(const char* file, const char* function,
            int line, const wchar_t* message)
        {
            std::wstringstream ss;
            ss << L"[" << function << L" @ " << ExtractFileName(file) << L":" << line << L"] " << message;
            return ss.str();
        }

        inline void SetDebugName(ID3D12Object* obj, const char* file,
            const char* function, int line, const wchar_t* message = L"")
        {
            if (!obj) return;
            auto name = BuildDebugName(file, function, line, message);
            obj->SetName(name.c_str());
        }

        inline void SetDebugName(IDXGIObject* obj, const char* file,
            const char* function, int line, const wchar_t* message = L"")
        {
            if (!obj) return;
            auto name = BuildDebugName(file, function, line, message);
            obj->SetPrivateData(WKPDID_D3DDebugObjectName, UINT(name.size()), name.c_str());
        }
    }

    #define d12SetDebugName(object, ...) \
        fge::debug::SetDebugName(object.Get(), __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
}
