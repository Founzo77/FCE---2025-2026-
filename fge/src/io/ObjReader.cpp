#include <fge/io/ObjReader.hpp>

#include <fge/io/MtlReader.hpp>
#include <fge/io/GlobalLogger.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

#include <charconv>
#include <fstream>
#include <filesystem>

namespace fge
{
    void ObjReader::read(const string& pathFile, const XMFLOAT3& translation, 
        const XMFLOAT3& rotation, const float scale, 
        Mesh& mesh, vector<Material>& materials, concurrent_vector<string>& texturePathFiles, 
        concurrent_unordered_map<string, uint32_t>& nameMaterialToTextureIndex,
        concurrent_unordered_map<string, uint32_t>& pathTextureToTextureIndex,
        unordered_map<string, uint32_t>& nameMaterialToMaterialIndex)
    {
        globalLogger().debug() << "Loading OBJ file: " << pathFile;

        std::ifstream file(pathFile);
        if (!file.is_open())
        {
            globalLogger().error() << "Failed to open OBJ file: " << pathFile;
            exit(EXIT_FAILURE);
        }

        vector<XMFLOAT3> positions;
        vector<XMFLOAT2> texcoords;
        vector<XMFLOAT3> normals;

        uint32_t currentMaterialIndex = UINT32_MAX;
        uint32_t currentIndexStart = UINT32_MAX;

        string directory = pathFile.substr(0, pathFile.find_last_of("/\\"));

        string line;
        while (std::getline(file, line))
        {
            std::istringstream lineStream(line);
            string prefix;
            lineStream >> prefix;

            if (prefix == "mtllib") // fichier MTL
            {
                string mtlFile;
                lineStream >> mtlFile;
                string completeMtlFile(directory + "/" + mtlFile);
                MtlReader::read(completeMtlFile, materials, texturePathFiles,
                    nameMaterialToTextureIndex, pathTextureToTextureIndex, 
                    nameMaterialToMaterialIndex);
            }
            else if (prefix == "usemtl") // on change de matériau
            {
                string materialName;
                lineStream >> materialName;

                if (currentIndexStart != UINT32_MAX)
                {
                    mesh.m_subMeshes.emplace_back(currentIndexStart,
                        static_cast<uint32_t>(mesh.m_indices.size() - currentIndexStart),
                        currentMaterialIndex);
                }

                currentIndexStart = static_cast<uint32_t>(mesh.m_indices.size());

                if (nameMaterialToMaterialIndex.find(materialName) != 
                    nameMaterialToMaterialIndex.end())
                {
                    currentMaterialIndex = nameMaterialToMaterialIndex[materialName];
                }
                else
                {
                    globalLogger().warning() << "Unknown material used: "
                        << materialName << " in OBJ: " << pathFile;
                    currentMaterialIndex = UINT32_MAX;
                }
            }
            else if (prefix == "v") // Vertex position
            {
                float x, y, z;
                lineStream >> x >> y >> z;
                positions.push_back({x, y, z});
            }
            else if (prefix == "vn") // Normales
            {
                float nx, ny, nz;
                lineStream >> nx >> ny >> nz;
                normals.push_back({nx, ny, nz});
            }
            else if (prefix == "vt") // Texture coordinates
            {
                float u, v;
                lineStream >> u >> v;
                v = 1.0f - v;
                texcoords.push_back({u, v});
            }
            else if (prefix == "f") // Face
            {
                auto parseFace = [&](const string &token) -> std::tuple<int,int,int>
                {
                    int vi = -1, ti = -1, ni = -1;

                    size_t firstSlash = token.find('/');
                    if (firstSlash == string::npos) 
                    {
                        vi = std::stoi(token) - 1;
                    } 
                    else 
                    {
                        vi = std::stoi(token.substr(0, firstSlash)) - 1;

                        size_t secondSlash = token.find('/', firstSlash + 1);
                        if (secondSlash == string::npos) 
                        {
                            // v/vt
                            ti = std::stoi(token.substr(firstSlash + 1)) - 1;
                        } 
                        else 
                        {
                            // v/vt/vn ou v//vn
                            if (secondSlash > firstSlash + 1) 
                            {
                                ti = std::stoi(token.substr(firstSlash + 1, 
                                    secondSlash - firstSlash - 1)) - 1;
                            }
                            if (secondSlash + 1 < token.size()) 
                            {
                                ni = std::stoi(token.substr(secondSlash + 1)) - 1;
                            }
                        }
                    }
                    return {vi, ti, ni};
                };

                std::vector<std::string> tokens;
                std::string token;
                while (lineStream >> token)
                {
                    tokens.push_back(token);
                }

                if (tokens.size() < 3)
                    continue;

                // parse tous les sommets
                std::vector<std::tuple<int,int,int>> parsed;
                for (auto& t : tokens)
                    parsed.push_back(parseFace(t));

                // triangulation en fan : (0, i, i+1)
                for (size_t i = 1; i+1 < parsed.size(); ++i)
                {
                    auto [vi1, ti1, ni1] = parsed[0];
                    auto [vi2, ti2, ni2] = parsed[i];
                    auto [vi3, ti3, ni3] = parsed[i+1];

                    Vertex vert1(
                        positions[vi1],
                        ni1 >= 0 ? normals[ni1] : XMFLOAT3{0,0,1},
                        ti1 >= 0 ? texcoords[ti1] : XMFLOAT2{0,0}
                    );

                    Vertex vert2(
                        positions[vi2],
                        ni2 >= 0 ? normals[ni2] : XMFLOAT3{0,0,1},
                        ti2 >= 0 ? texcoords[ti2] : XMFLOAT2{0,0}
                    );

                    Vertex vert3(
                        positions[vi3],
                        ni3 >= 0 ? normals[ni3] : XMFLOAT3{0,0,1},
                        ti3 >= 0 ? texcoords[ti3] : XMFLOAT2{0,0}
                    );

                    // Transformation
                    XMMATRIX transform =
                        XMMatrixScaling(scale, scale, scale) *
                        XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) *
                        XMMatrixTranslation(translation.x, translation.y, translation.z);

                    auto applyTransform = [&](Vertex &v)
                    {
                        XMVECTOR pos = XMLoadFloat3(&v.m_position);
                        pos = XMVector3Transform(pos, transform);
                        XMStoreFloat3(&v.m_position, pos);
                    };

                    auto applyTransformNormal = [&](Vertex &v)
                    {
                        XMVECTOR n = XMLoadFloat3(&v.m_normal);
                        n = XMVector3TransformNormal(n, transform);
                        n = XMVector3Normalize(n);
                        XMStoreFloat3(&v.m_normal, n);
                    };

                    applyTransform(vert1);
                    applyTransform(vert2);
                    applyTransform(vert3);
                    applyTransformNormal(vert1);
                    applyTransformNormal(vert2);
                    applyTransformNormal(vert3);

                    uint32_t baseIndex = static_cast<uint32_t>(mesh.m_vertices.size());
                    mesh.m_vertices.push_back(vert1);
                    mesh.m_vertices.push_back(vert2);
                    mesh.m_vertices.push_back(vert3);

                    mesh.m_indices.push_back(baseIndex);
                    mesh.m_indices.push_back(baseIndex + 1);
                    mesh.m_indices.push_back(baseIndex + 2);
                }
            }
        }

        if (currentIndexStart != UINT32_MAX)
        {
            mesh.m_subMeshes.emplace_back(currentIndexStart,
                static_cast<uint32_t>(mesh.m_indices.size() - currentIndexStart),
                currentMaterialIndex);
        }

        globalLogger().debug() << "OBJ successfully loaded: " << pathFile;
    }
}