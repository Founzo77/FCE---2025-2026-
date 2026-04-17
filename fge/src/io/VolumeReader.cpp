#include <fge/io/VolumeReader.hpp>
#include <fge/io/GlobalLogger.hpp>

#include <fstream>
#include <vector>
#include <cstdint>
#include <cassert>
#include <algorithm>
#include <filesystem>

namespace fge
{
    static inline uint16_t swap16(uint16_t v) { return (v >> 8) | (v << 8); }

    static inline size_t bytesPerVoxel(DXGI_FORMAT format)
    {
        switch (format)
        {
            case DXGI_FORMAT_R8_UNORM:  return 1;
            case DXGI_FORMAT_R16_UNORM: return 2;
            case DXGI_FORMAT_R16_SNORM: return 2;
            case DXGI_FORMAT_R32_FLOAT: return 4;
            default: return 0;
        }
    }

    struct CubeParsed
    {
        int nx = 0, ny = 0, nz = 0;
        std::array<float,3> origin{0,0,0};
        std::array<float,3> vx{1,0,0}; // axis step vector for X
        std::array<float,3> vy{0,1,0};
        std::array<float,3> vz{0,0,1};
        std::vector<float> values;     // size nx*ny*nz
    };

    static CubeParsed parseCubeFile(const std::string& pathFile)
    {
        std::ifstream in(pathFile);
        if (!in.is_open())
        {
            globalLogger().error() << "Failed to open .cube file: " << pathFile;
            exit(EXIT_FAILURE);
        }

        CubeParsed out{};

        std::string line;

        // 1) Two comment lines
        std::getline(in, line);
        std::getline(in, line);

        // 2) Third line: NAtoms + origin
        int nAtoms = 0;
        if (!(in >> nAtoms >> out.origin[0] >> out.origin[1] >> out.origin[2]))
        {
            globalLogger().error() << "Invalid .cube header (atoms/origin): " << pathFile;
            exit(EXIT_FAILURE);
        }

        // 3) Axis lines: Nx vx ; Ny vy ; Nz vz
        if (!(in >> out.nx >> out.vx[0] >> out.vx[1] >> out.vx[2]))
        {
            globalLogger().error() << "Invalid .cube axis X line: " << pathFile;
            exit(EXIT_FAILURE);
        }
        if (!(in >> out.ny >> out.vy[0] >> out.vy[1] >> out.vy[2]))
        {
            globalLogger().error() << "Invalid .cube axis Y line: " << pathFile;
            exit(EXIT_FAILURE);
        }
        if (!(in >> out.nz >> out.vz[0] >> out.vz[1] >> out.vz[2]))
        {
            globalLogger().error() << "Invalid .cube axis Z line: " << pathFile;
            exit(EXIT_FAILURE);
        }

        const int atomLines = std::abs(nAtoms);

        // 4) Skip atom lines: atomicNumber charge x y z
        for (int i = 0; i < atomLines; ++i)
        {
            int atomicNumber = 0;
            float charge = 0, x = 0, y = 0, z = 0;
            if (!(in >> atomicNumber >> charge >> x >> y >> z))
            {
                globalLogger().error() << "Invalid .cube atom line: " << pathFile;
                exit(EXIT_FAILURE);
            }
        }

        // 5) Read volumetric values
        const size_t voxelCount = static_cast<size_t>(out.nx) * out.ny * out.nz;
        out.values.resize(voxelCount);

        for (size_t i = 0; i < voxelCount; ++i)
        {
            if (!(in >> out.values[i]))
            {
                globalLogger().error() << "Not enough volumetric values in .cube: " << pathFile;
                exit(EXIT_FAILURE);
            }
        }

        return out;
    }

    // Convert float field -> target format buffer (bytes)
    static std::vector<uint8_t> convertScalarFieldToBytes(
        const std::vector<float>& src, DXGI_FORMAT format, float scalarScale,
        bool normalizeToUNorm // for UNORM targets: normalize by min/max
    )
    {
        const size_t voxelCount = src.size();
        const size_t bpp = bytesPerVoxel(format);
        if (bpp == 0)
        {
            globalLogger().error() << "Unsupported DXGI_FORMAT for volume conversion: " << (int)format;
            exit(EXIT_FAILURE);
        }

        // Apply scale and compute min/max (for normalization)
        float minV = +std::numeric_limits<float>::infinity();
        float maxV = -std::numeric_limits<float>::infinity();

        std::vector<float> scaled(voxelCount);
        for (size_t i = 0; i < voxelCount; ++i)
        {
            float v = src[i] * scalarScale;
            scaled[i] = v;
            minV = std::min(minV, v);
            maxV = std::max(maxV, v);
        }

        const float range = maxV - minV;
        auto norm01 = [&](float v) -> float
        {
            if (!normalizeToUNorm) return std::clamp(v, 0.0f, 1.0f);
            if (range <= 1e-20f) return 0.0f;
            return std::clamp((v - minV) / range, 0.0f, 1.0f);
        };

        std::vector<uint8_t> outBytes(voxelCount * bpp);

        if (format == DXGI_FORMAT_R32_FLOAT)
        {
            std::memcpy(outBytes.data(), scaled.data(), voxelCount * sizeof(float));
            return outBytes;
        }
        else if (format == DXGI_FORMAT_R16_UNORM)
        {
            auto* dst = reinterpret_cast<uint16_t*>(outBytes.data());
            for (size_t i = 0; i < voxelCount; ++i)
            {
                float u = norm01(scaled[i]);
                float v = u * 65535.0f;
                dst[i] = static_cast<uint16_t>(std::lround(std::clamp(v, 0.0f, 65535.0f)));
            }
            return outBytes;
        }
        else if (format == DXGI_FORMAT_R8_UNORM)
        {
            auto* dst = reinterpret_cast<uint8_t*>(outBytes.data());
            for (size_t i = 0; i < voxelCount; ++i)
            {
                float u = norm01(scaled[i]);
                float v = u * 255.0f;
                dst[i] = static_cast<uint8_t>(std::lround(std::clamp(v, 0.0f, 255.0f)));
            }
            return outBytes;
        }
        else if (format == DXGI_FORMAT_R16_SNORM)
        {
            // SNORM expects [-1..1] mapped to int16 range.
            // We normalize around min/max into [-1..1] (simple approach).
            auto* dst = reinterpret_cast<int16_t*>(outBytes.data());
            for (size_t i = 0; i < voxelCount; ++i)
            {
                float u = normalizeToUNorm ? (range <= 1e-20f ? 0.0f : (scaled[i] - minV) / range) : scaled[i];
                // map u in [0..1] to [-1..1]
                float s = std::clamp(u * 2.0f - 1.0f, -1.0f, 1.0f);
                float v = s * 32767.0f;
                dst[i] = static_cast<int16_t>(std::lround(std::clamp(v, -32768.0f, 32767.0f)));
            }
            return outBytes;
        }

        globalLogger().error() << "Unhandled DXGI_FORMAT conversion case: " << (int)format;
        exit(EXIT_FAILURE);
        return outBytes;
    }

    void VolumeReader::read(const string& pathFile, const string& transfertFunctionPathFile, 
        const uint32_t width, const uint32_t height, const uint32_t depth, 
        const array<float, 3> volumeMin, const array<float, 3> volumeMax, 
        const DXGI_FORMAT format, const bool isLittleEndian, const float scalarScale, 
        Volume& volume, Texture& texture3D,
        concurrent_vector<string>& texturePathFiles, 
        concurrent_unordered_map<string, uint32_t>& pathTextureToTextureIndex)
    {
        const std::string extension = std::filesystem::path(pathFile).extension().string();

        if (extension == ".raw" || extension == ".RAW")
        {
            readRawVolume(pathFile, transfertFunctionPathFile, width, height, depth, volumeMin, volumeMax,
                format, isLittleEndian, scalarScale, volume, texture3D);
        }
        else if(extension == ".cube" || extension == ".CUBE")
        {
            readRawCube(pathFile, transfertFunctionPathFile, width, height, depth, volumeMin, volumeMax,
                format, isLittleEndian, scalarScale, volume, texture3D);
        }
        else
        {
            globalLogger().error() << "Unsupported volume extension for file: " << pathFile
                << " (expected .raw or .cube)";
            exit(EXIT_FAILURE);
        }

        if(std::all_of(transfertFunctionPathFile.begin(),
                transfertFunctionPathFile.end(),
            [](unsigned char c){ return std::isspace(c); }))
        {
            volume.m_volumeData.m_transferFunctionTextureIndex = UINT32_MAX;
        }
        else
        {
            uint32_t textureIndex;
            #pragma omp critical
            {
                texturePathFiles.push_back(transfertFunctionPathFile);
                textureIndex = static_cast<uint32_t>(texturePathFiles.size() - 1);
                pathTextureToTextureIndex[transfertFunctionPathFile] = textureIndex;
            }
            volume.m_volumeData.m_transferFunctionTextureIndex = textureIndex;
        }
    }

    void VolumeReader::readRawVolume(const string& pathFile, const string& transfertFunctionPathFile, 
        const uint32_t width, const uint32_t height, const uint32_t depth, 
        const array<float, 3> volumeMin, const array<float, 3> volumeMax, 
        const DXGI_FORMAT format, const bool isLittleEndian, const float scalarScale, 
        Volume& volume, Texture& texture3D)
    {
        const size_t bpp = bytesPerVoxel(format);
        if (bpp == 0)
        {
            globalLogger().error() << "Unsupported DXGI_FORMAT for volume: " << (int)format;
            exit(EXIT_FAILURE);
        }

        const size_t voxelCount = static_cast<size_t>(width) * height * depth;
        const size_t byteCount = voxelCount * bpp;

        std::ifstream file(pathFile, std::ios::binary);
        if (!file.is_open())
        {
            globalLogger().error() << "Failed to open volume file: " << pathFile;
            exit(EXIT_FAILURE);
        }

        std::vector<uint8_t> bytes(byteCount);
        file.read(reinterpret_cast<char*>(bytes.data()), byteCount);

        if (static_cast<size_t>(file.gcount()) != byteCount)
        {
            globalLogger().error() << "Volume file size mismatch: expected "
                << byteCount << " bytes, got " << file.gcount()
                << " bytes. File: " << pathFile;
            exit(EXIT_FAILURE);
        }

        // Si le fichier est big-endian et qu'on est sur machine little-endian,
        // on swap uniquement pour des voxels 16-bit (ou 32-bit selon tes formats)
        // Ici ton CT head = 16-bit big-endian.
        if (!isLittleEndian)
        {
            if (format == DXGI_FORMAT_R16_UNORM || format == DXGI_FORMAT_R16_SNORM)
            {
                auto* p = reinterpret_cast<uint16_t*>(bytes.data());
                for (size_t i = 0; i < voxelCount; ++i)
                    p[i] = swap16(p[i]);
            }
            else if (format == DXGI_FORMAT_R32_FLOAT)
            {
                // swap32 si un jour tu as des float big-endian
                auto* p = reinterpret_cast<uint32_t*>(bytes.data());
                for (size_t i = 0; i < voxelCount; ++i)
                {
                    uint32_t v = p[i];
                    p[i] = (v >> 24) |
                           ((v >> 8) & 0x0000FF00u) |
                           ((v << 8) & 0x00FF0000u) |
                           (v << 24);
                }
            }
        }

        if (format == DXGI_FORMAT_R16_UNORM)
        {
            auto* p = reinterpret_cast<uint16_t*>(bytes.data());

            for (size_t i = 0; i < voxelCount; ++i)
            {
                float v = static_cast<float>(p[i]);
                v *= scalarScale;

                // clamp pour éviter overflow
                v = std::clamp(v, 0.0f, 65535.0f);

                p[i] = static_cast<uint16_t>(v);
            }
        }
        else if (format == DXGI_FORMAT_R16_SNORM)
        {
            auto* p = reinterpret_cast<int16_t*>(bytes.data());

            for (size_t i = 0; i < voxelCount; ++i)
            {
                float v = static_cast<float>(p[i]);
                v *= scalarScale;

                v = std::clamp(v, -32768.0f, 32767.0f);
                p[i] = static_cast<int16_t>(v);
            }
        }
        else if (format == DXGI_FORMAT_R32_FLOAT)
        {
            auto* p = reinterpret_cast<float*>(bytes.data());

            for (size_t i = 0; i < voxelCount; ++i)
            {
                p[i] *= scalarScale;
            }
        }
        else if (format == DXGI_FORMAT_R8_UNORM)
        {
            auto* p = reinterpret_cast<uint8_t*>(bytes.data());

            for (size_t i = 0; i < voxelCount; ++i)
            {
                float v = static_cast<float>(p[i]);
                v *= scalarScale;

                // clamp UNORM 8-bit
                v = std::clamp(v, 0.0f, 255.0f);

                p[i] = static_cast<uint8_t>(v);
            }
        }

        volume.m_volumeData.m_mins = volumeMin;
        volume.m_volumeData.m_maxs = volumeMax;

        texture3D.initializeFromRawVolume(bytes.data(), width, height, depth, format);
    }

    void VolumeReader::readRawCube(const string& pathFile, const string& transfertFunctionPathFile, 
        const uint32_t width, const uint32_t height, const uint32_t depth, 
        const array<float, 3> volumeMin, const array<float, 3> volumeMax, 
        const DXGI_FORMAT format, const bool isLittleEndian, const float scalarScale, 
        Volume& volume, Texture& texture3D)
    {
        CubeParsed cube = parseCubeFile(pathFile);

        if (width != 0 && height != 0 && depth != 0)
        {
            if (static_cast<int>(width) != cube.nx || static_cast<int>(height) != cube.ny ||
                static_cast<int>(depth) != cube.nz)
            {
                globalLogger().warning()
                    << ".cube dims (" << cube.nx << "," << cube.ny << "," << cube.nz
                    << ") differ from XML dims (" << width << "," << height << "," << depth
                    << "). Using .cube dims.";
            }
        }

        const bool normalizeToUNorm = 
            (format == DXGI_FORMAT_R16_UNORM || format == DXGI_FORMAT_R8_UNORM);
        std::vector<uint8_t> bytes = convertScalarFieldToBytes(
            cube.values, format, scalarScale, normalizeToUNorm);

        texture3D.initializeFromRawVolume(bytes.data(), static_cast<uint32_t>(cube.nx), 
            static_cast<uint32_t>(cube.ny), static_cast<uint32_t>(cube.nz), format);

        volume.m_volumeData.m_mins = volumeMin;
        volume.m_volumeData.m_maxs = volumeMax;

        globalLogger().debug() << "Loaded .cube volume: " << pathFile
            << " dims=" << cube.nx << "x" << cube.ny << "x" << cube.nz
            << " origin=(" << cube.origin[0] << "," << cube.origin[1] << "," << cube.origin[2] << ")";
    }
}
