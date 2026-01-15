#include <fge/resources/ShaderFactory.hpp>

#include <fge/utility.hpp>

#include <fstream>

namespace fge
{
    vector<char> ShaderFactory::loadShader(const string& filePath)
    {
        std::ifstream fs(filePath, std::ios::binary | std::ios::ate);
        throwIfFailed(fs.is_open(), "Failed to open file");
        size_t size = fs.tellg();
        throwIfFailed(size > 0, "Shader file is empty or unreadable");
        std::vector<char> buffer(size);
        fs.seekg(0);
        throwIfFailed((!fs.read(buffer.data(), size) == false), "Failed to read file");

        return buffer;
    }
}