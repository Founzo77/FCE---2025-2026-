#include <fge/resources/ShaderFactory.hpp>

#include <fge/utility.hpp>

#include <fstream>

#include <windows.h>
#include <filesystem>

namespace fge
{
    static std::filesystem::path getModuleDirectory()
    {
        HMODULE module = nullptr;

        GetModuleHandleEx(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCSTR>(&getModuleDirectory),
            &module);

        wchar_t path[MAX_PATH];
        GetModuleFileNameW(module, path, MAX_PATH);

        return std::filesystem::path(path).parent_path();
    }

    vector<char> ShaderFactory::loadShader(const string& filePath)
    {
        static std::filesystem::path base = getModuleDirectory();

        std::filesystem::path fullPath = base / filePath;

        std::ifstream fs(fullPath, std::ios::binary | std::ios::ate);
        throwIfFailed(fs.is_open(), "Failed to open file");
        size_t size = fs.tellg();
        throwIfFailed(size > 0, "Shader file is empty or unreadable");
        std::vector<char> buffer(size);
        fs.seekg(0);
        throwIfFailed((!fs.read(buffer.data(), size) == false), "Failed to read file");

        return buffer;
    }
}