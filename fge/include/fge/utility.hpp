#pragma once

#include <Windows.h>

#include <exception>
#include <cstdint>
#include <string>

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
            throw std::exception();
        }
    }

    inline void throwIfFailed(HRESULT hResult, const char* message)
    {
        if(FAILED(hResult))
        {
            throw std::exception(message);
        }
    }

    inline void throwIfFailed(HANDLE handle)
    {
        if(!handle)
        {
            throw std::exception();
        }
    }

    inline void throwIfFailed(HANDLE handle, const char* message)
    {
        if(!handle)
        {
            throw std::exception(message);
        }
    }

    inline void throwIfFailed(bool handle)
    {
        if(!handle)
        {
            throw std::exception();
        }
    }

    inline void throwIfFailed(bool handle, const char* message)
    {
        if(!handle)
        {
            throw std::exception(message);
        }
    }

    inline void throwIfFailed(bool handle, const std::string& message)
    {
        if(!handle)
        {
            throw std::exception(message.c_str());
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
}