#include <fge/io/MtlReader.hpp>
#include <fge/io/GlobalLogger.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

namespace fge
{
    MtlReader::MtlReader(const string& pathFile)
    {
        MtlReader::read(pathFile, m_materials, m_texturePathFiles, m_nameMaterialToTextureIndex,
            m_pathTextureToTextureIndex, m_nameMaterialToMaterialIndex);
    }

    void MtlReader::read(const string& pathFile, vector<Material>& materials, 
        vector<string>& texturePathFiles, 
        unordered_map<string, uint32_t>& nameMaterialToTextureIndex,
        unordered_map<string, uint32_t>& pathTextureToTextureIndex,
        unordered_map<string, uint32_t>& nameMaterialToMaterialIndex)
    {
        globalLogger().debug() << "Loading MTL file: " << pathFile;

        std::ifstream mtlFile(pathFile);
        if (!mtlFile.is_open())
        {
            globalLogger().error() << "Failed to open MTL file: " << pathFile;
            exit(EXIT_FAILURE);
        }

        string line, currentMat;

        string directory = pathFile.substr(0, pathFile.find_last_of("/\\"));

        while (std::getline(mtlFile, line))
        {
            std::istringstream fileStream(line);
            string prefix;
            fileStream >> prefix;

            if (prefix == "newmtl")
            {
                fileStream >> currentMat;

                Material material;
                uint32_t materialIndex = static_cast<uint32_t>(materials.size());
                materials.push_back(material);

                nameMaterialToMaterialIndex[currentMat] = materialIndex;
            }
            else if (prefix == "map_Kd" && !currentMat.empty())
            {
                string texturePathFile;
                fileStream >> texturePathFile;

                string completeTexturePathFile(directory + "/" + texturePathFile);

                if(pathTextureToTextureIndex.find(completeTexturePathFile) == 
                    pathTextureToTextureIndex.end())
                {
                    texturePathFiles.push_back(completeTexturePathFile);
                    uint32_t textureIndex = 
                        static_cast<uint32_t>(texturePathFiles.size() - 1);

                    nameMaterialToTextureIndex[currentMat] = textureIndex;
                    pathTextureToTextureIndex[completeTexturePathFile] = textureIndex;
                }
                else
                {
                    nameMaterialToTextureIndex[currentMat] = 
                        pathTextureToTextureIndex[completeTexturePathFile];
                }

                uint32_t textureIndex = nameMaterialToTextureIndex[currentMat];

                uint32_t materialIndex = nameMaterialToMaterialIndex[currentMat];
                materials[materialIndex].m_albedoTextureIndex = textureIndex;
            }
            else if ((prefix == "map_Bump" || prefix == "map_bump" || 
                prefix == "bump") && !currentMat.empty())
            {
                std::string token;
                std::string texturePathFile;

                // Parcourir tous les tokens restants de la ligne
                while (fileStream >> token)
                {
                    if (token[0] == '-') // option (comme -bm 1.0), on saute la valeur qui suit
                    {
                        std::string dummy;
                        fileStream >> dummy; // lire et ignorer la valeur après -bm
                    }
                    else
                    {
                        texturePathFile = token; // premier token qui n'est pas une option = fichier
                        break;
                    }
                }

                if (!texturePathFile.empty())
                {
                    std::string completeTexturePathFile = directory + "/" + texturePathFile;

                    if(pathTextureToTextureIndex.find(completeTexturePathFile) == 
                    pathTextureToTextureIndex.end())
                    {
                        texturePathFiles.push_back(completeTexturePathFile);
                        uint32_t textureIndex = static_cast<uint32_t>
                            (texturePathFiles.size() - 1);

                        // Ici il faut stocker séparément l’albedo et la normal
                        // Donc on ne va PAS écraser nameMaterialToTextureIndex, 
                        // on doit avoir une map dédiée
                        nameMaterialToTextureIndex[currentMat + "_normal"] = textureIndex;
                        pathTextureToTextureIndex[completeTexturePathFile] = textureIndex;
                    }
                    else
                    {
                        nameMaterialToTextureIndex[currentMat + "_normal"] =
                            pathTextureToTextureIndex[completeTexturePathFile];
                    }

                    uint32_t textureIndex = nameMaterialToTextureIndex[currentMat + "_normal"];

                    uint32_t materialIndex = nameMaterialToMaterialIndex[currentMat];
                    materials[materialIndex].m_normalTextureIndex = textureIndex;
                }
            }
            else if (prefix == "reflectance" && !currentMat.empty())
            {
                float r, g, b;
                fileStream >> r >> g >> b;

                uint32_t materialIndex = nameMaterialToMaterialIndex[currentMat];
                materials[materialIndex].m_reflectance = XMFLOAT3(r, g, b);
            }
            else if (prefix == "transmittance" && !currentMat.empty())
            {
                float r, g, b;
                fileStream >> r >> g >> b;

                uint32_t materialIndex = nameMaterialToMaterialIndex[currentMat];
                materials[materialIndex].m_transmittance = XMFLOAT3(r, g, b);
            }
            else if (prefix == "Ks" && !currentMat.empty())
            {
                float r, g, b;
                fileStream >> r >> g >> b;

                uint32_t materialIndex = nameMaterialToMaterialIndex[currentMat];
                materials[materialIndex].m_ks = XMFLOAT3(r, g, b);
            }
            else if (prefix == "Ns" && !currentMat.empty())
            {
                float ns;
                fileStream >> ns;

                uint32_t materialIndex = nameMaterialToMaterialIndex[currentMat];
                materials[materialIndex].m_shininess = ns;
            }
        }

        globalLogger().debug() << "MTL successfully loaded: " << pathFile;
    }
}