#include <fge/io/XmlReader.hpp>
#include <fge/io/ObjReader.hpp>
#include <fge/io/VolumeReader.hpp>
#include <fge/io/GlobalLogger.hpp>

#include <fge/render/pipeline_state/ShaderBank.hpp>

#include <iostream>


using std::wstring;

namespace fge
{
    // TO_DO Mettre ca dans utility
    static std::wstring toWString(const char* utf8Str)
    {
        if (!utf8Str)
            return {};

        int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, nullptr, 0);

        if (sizeNeeded <= 0)
            return {};

        std::wstring result(sizeNeeded - 1, L'\0');

        MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, result.data(), sizeNeeded);

        return result;
    }


    XmlReader::XmlReader(const std::string& pathFile)
    {
        globalLogger().debug() << "Loading scene from file: " << pathFile;

        tinyxml2::XMLDocument document;

        if (document.LoadFile(pathFile.c_str()) != tinyxml2::XML_SUCCESS) 
        {
            globalLogger().error() << "Failed to load XML file: " << pathFile;
            globalLogger().error() << "TinyXML2 error: " << document.ErrorStr();
            exit(EXIT_FAILURE);
        }

        tinyxml2::XMLElement* root = document.FirstChildElement("scene");

        if(!root)
        {
            globalLogger().error() << "Root element <scene> not found!";
            exit(EXIT_FAILURE);
        }

        loadAll(*root);

        globalLogger().debug() << "Scene successfully loaded: " << pathFile;
    }

    XmlReader::XmlReader(tinyxml2::XMLElement& root)
    {
        loadAll(root);
        globalLogger().debug() << "Render scene successfully loaded";
    }

    void XmlReader::loadAll(tinyxml2::XMLElement& root)
    {
        loadBackground(root);
        loadCamera(root);
        loadEmitters(root);
        loadMeshes(root);
        loadVolumes(root);
        loadTextures();
        loadInstances(root);
        loadRenderConfig(root);
    }

    void XmlReader::loadBackground(tinyxml2::XMLElement& root)
    {
        tinyxml2::XMLElement* background = root.FirstChildElement("background");

        if(background)
        {
            const char* texturePathCStr = background->Attribute("texture_path");

            if (texturePathCStr)
            {
                std::string texturePath = texturePathCStr;

                auto itTexturePath = m_pathTextureToTextureIndex.find(texturePath);
                uint32_t textureIndex;

                if (itTexturePath != m_pathTextureToTextureIndex.end())
                {
                    textureIndex = itTexturePath->second;
                }
                else
                {
                    textureIndex = static_cast<uint32_t>(m_texturePathFiles.size());
                    m_texturePathFiles.push_back(texturePath);
                    m_pathTextureToTextureIndex[texturePath] = textureIndex;
                }

                m_sceneDescription.m_background = Background(textureIndex);
            }
            else
            {
                m_sceneDescription.m_background = Background();
            }
        }
        else
        {
            globalLogger().error() << "No <background> element found!";
            exit(EXIT_FAILURE);
        }
    }

    void XmlReader::loadCamera(tinyxml2::XMLElement& root)
    {
        tinyxml2::XMLElement* customCamera = root.FirstChildElement("perspective_camera");

        if(customCamera)
        {
            XMFLOAT3 origin = loadVector3D(*customCamera, "origin");
            XMFLOAT3 forward = loadVector3D(*customCamera, "forward");
            XMFLOAT3 up = loadVector3D(*customCamera, "up");

            float depth = 0;
            float width = 0;
            float height = 0;
            float sizePixel = 0;

            customCamera->QueryFloatAttribute("depth", &depth);
            customCamera->QueryFloatAttribute("width", &width);
            customCamera->QueryFloatAttribute("height", &height);
            customCamera->QueryFloatAttribute("size_pixel", &sizePixel);

            // TO_DO voir pour sizePixel
            m_sceneDescription.m_camera = Camera(origin, forward, up, width, height, depth);
        }
        else
        {
            globalLogger().error() << "No <perspective_camera> element found!";
            exit(EXIT_FAILURE);
        }
    }

    void XmlReader::loadEmitters(tinyxml2::XMLElement& root)
    {
        tinyxml2::XMLElement* emittersElement = root.FirstChildElement("emitters");
    
        if(emittersElement)
        {
            for (tinyxml2::XMLElement* light = 
                emittersElement->FirstChildElement("light"); 
                light != nullptr; 
                light = light->NextSiblingElement("light")) 
            {
                XMFLOAT3 radiance = loadVector3D(*light, string("radiance"));
                XMFLOAT3 position = loadVector3D(*light, string("position"));

                m_sceneDescription.m_lights.push_back(Light(radiance, position));
            }
        }
        else
        {
            globalLogger().error() << "No <emitters> element found!";
            exit(EXIT_FAILURE);
        }
    }

    void XmlReader::loadMeshes(tinyxml2::XMLElement& root)
    {
        tinyxml2::XMLElement* objectsElement = root.FirstChildElement("meshes");
    
        if(objectsElement)
        {
            // TO_DO Rajouter les triangles et cercles

            concurrent_unordered_map<string, uint32_t> nameMaterialToTextureIndex;

            std::vector<tinyxml2::XMLElement*> includes;

            for (tinyxml2::XMLElement* includeFile =
                    objectsElement->FirstChildElement("include");
                includeFile != nullptr;
                includeFile = includeFile->NextSiblingElement("include"))
            {
                includes.push_back(includeFile);
            }

            #pragma omp parallel for schedule(dynamic)
            for (int i = 0; i < static_cast<int>(includes.size()); i++) 
            {
                tinyxml2::XMLElement* includeFile = includes[i];

                const char* path = includeFile->Attribute("path");

                XMFLOAT3 translation = loadVector3D(*includeFile, 
                    string("translation"));
                XMFLOAT3 rotation = loadVector3D(*includeFile,
                    string("rotation"));

                float scale = 1;
                includeFile->QueryFloatAttribute("scale", &scale);
                int id = 0;
                includeFile->QueryIntAttribute("id", &id);

                Mesh mesh;
                unordered_map<string, uint32_t> nameMaterialToMaterialIndex;

                ObjReader::read(string(path), translation, rotation, scale,
                    mesh, m_sceneDescription.m_materials, m_texturePathFiles, 
                    nameMaterialToTextureIndex,
                    m_pathTextureToTextureIndex, nameMaterialToMaterialIndex);
                
                const char* hitGroupNameStr = includeFile->Attribute("hit_group");
                if (!hitGroupNameStr)
                {
                    globalLogger().error() << "Volume include id=" << id << 
                        " missing 'path' attribute!";
                    
                    exit(EXIT_FAILURE);
                }
                const wstring hitGroupName = toWString(hitGroupNameStr);
                mesh.m_hitGroupIndex = ShaderBank::getHitGroupIndex(hitGroupName);
                
                #pragma omp critical
                {
                    m_sceneDescription.m_meshes.insert({ LogicalIndex(id), std::move(mesh) });
                }
            }
        }
        else
        {
            globalLogger().error() << "No <meshes> element found!";
            exit(EXIT_FAILURE);
        }
    }

    void XmlReader::loadVolumes(tinyxml2::XMLElement& root)
    {
        tinyxml2::XMLElement* volumesElement = root.FirstChildElement("volumes");
    
        if(volumesElement)
        {
            std::vector<tinyxml2::XMLElement*> includes;

            for (tinyxml2::XMLElement* includeFile =
                    volumesElement->FirstChildElement("include");
                includeFile != nullptr;
                includeFile = includeFile->NextSiblingElement("include"))
            {
                includes.push_back(includeFile);
            }

            #pragma omp parallel for schedule(dynamic)
            for (int i = 0; i < static_cast<int>(includes.size()); i++)
            {
                tinyxml2::XMLElement* includeFile = includes[i];

                int id = 0;
                if (includeFile->QueryIntAttribute("id", &id) != tinyxml2::XML_SUCCESS)
                {
                    globalLogger().error() << "<include> in <volumes> missing 'id' attribute!";
                    exit(EXIT_FAILURE);
                }

                const char* pathCstr = includeFile->Attribute("path");
                if (!pathCstr)
                {
                    globalLogger().error() << "Volume include id=" << id << 
                        " missing 'path' attribute!";
                    exit(EXIT_FAILURE);
                }
                const string path = string(pathCstr);

                const char* pathTFCstr = includeFile->Attribute("transfert_function_path");
                if (!pathTFCstr)
                {
                    globalLogger().error() << "Volume include id=" << id << 
                        " missing 'transfert_function_path' attribute!";
                    exit(EXIT_FAILURE);
                }
                const string pathTF = string(pathTFCstr);

                // dims
                uint32_t width = 0, height = 0, depth = 0;
                if (includeFile->QueryUnsignedAttribute("width", &width) != tinyxml2::XML_SUCCESS ||
                    includeFile->QueryUnsignedAttribute("height", &height) != tinyxml2::XML_SUCCESS ||
                    includeFile->QueryUnsignedAttribute("depth", &depth) != tinyxml2::XML_SUCCESS)
                {
                    globalLogger().error() << "Volume include id=" << id
                        << " must have width/height/depth attributes!";
                    exit(EXIT_FAILURE);
                }

                const bool isLittleEndian = loadBool(*includeFile, "is_little_endian");

                DXGI_FORMAT dxgiFormat = loadFormat(*includeFile, "format");

                // volume_min / volume_max
                array<float, 3> vmin{ 0.f, 0.f, 0.f };
                array<float, 3> vmax{ 1.f, 1.f, 1.f };

                if (tinyxml2::XMLElement* minEl = includeFile->FirstChildElement("volume_min"))
                {
                    minEl->QueryFloatAttribute("x", &vmin[0]);
                    minEl->QueryFloatAttribute("y", &vmin[1]);
                    minEl->QueryFloatAttribute("z", &vmin[2]);
                }
                else
                {
                    globalLogger().error() << "Volume include id=" << id << " missing <volume_min>!";
                    exit(EXIT_FAILURE);
                }

                if (tinyxml2::XMLElement* maxEl = includeFile->FirstChildElement("volume_max"))
                {
                    maxEl->QueryFloatAttribute("x", &vmax[0]);
                    maxEl->QueryFloatAttribute("y", &vmax[1]);
                    maxEl->QueryFloatAttribute("z", &vmax[2]);
                }
                else
                {
                    globalLogger().error() << "Volume include id=" << id << " missing <volume_max>!";
                    exit(EXIT_FAILURE);
                }

                float scalarScale = 1;
                includeFile->QueryFloatAttribute("scalar_scale", &scalarScale);

                Texture texture3D;
                Volume volume;

                VolumeReader::read(path, pathTF, width, height, depth, vmin, vmax, 
                    dxgiFormat, isLittleEndian, scalarScale,
                    volume, texture3D, m_texturePathFiles, m_pathTextureToTextureIndex);

                float scalarMin = 0;
                includeFile->QueryFloatAttribute("scalar_min", &scalarMin);
                float scalarMax = 1;
                includeFile->QueryFloatAttribute("scalar_max", &scalarMax);
                volume.m_volumeData.m_scalarMin = scalarMin;
                volume.m_volumeData.m_scalarMax = scalarMax;

                const char* hitGroupNameStr = includeFile->Attribute("hit_group");
                if (!hitGroupNameStr)
                {
                    globalLogger().error() << "Volume include id=" << id << 
                        " missing 'path' attribute!";
                    exit(EXIT_FAILURE);
                }
                const wstring hitGroupName = toWString(hitGroupNameStr);
                volume.m_hitGroupIndex = ShaderBank::getHitGroupIndex(hitGroupName);

                #pragma omp critical(add)
                {
                    const uint32_t textureIndex = 
                        static_cast<uint32_t>(m_sceneDescription.m_textures3D.size());
                    volume.m_volumeData.m_texture3DIndex = textureIndex;

                    m_sceneDescription.m_volumes.insert({ LogicalIndex(id), std::move(volume) });
                    m_sceneDescription.m_textures3D.push_back(std::move(texture3D));
                    globalLogger().debug() << "Texture3D[" << m_sceneDescription.m_textures3D.size() - 1 
                        << "] loaded: " << path.c_str();
                }
            }
        }
        else
        {
            globalLogger().error() << "No <volumes> element found!";
            exit(EXIT_FAILURE);
        }
    }

    void XmlReader::loadInstances(tinyxml2::XMLElement& root)
    {
        tinyxml2::XMLElement* objectsElement = root.FirstChildElement("instances");
    
        if(objectsElement)
        {
            for (tinyxml2::XMLElement* includeFile = 
                objectsElement->FirstChildElement("instance"); 
                includeFile != nullptr;
                includeFile = includeFile->NextSiblingElement("instance")) 
            {

                const char* path = includeFile->Attribute("path");

                XMFLOAT3 translation = loadVector3D(*includeFile, string("translation"));
                XMFLOAT3 rotation = loadVector3D(*includeFile, string("rotation"));
                XMFLOAT3 scale = loadVector3D(*includeFile, string("scale"));

                int geometry_id = 0;
                includeFile->QueryIntAttribute("geometry_id", &geometry_id);
                int id = 0;
                includeFile->QueryIntAttribute("id", &id);

                const char* typeStr = includeFile->Attribute("type");
                if (!typeStr)
                {
                    globalLogger().error() << "Instance has no type attribute!";
                    exit(EXIT_FAILURE);
                }

                GeometryType geometryType;

                if (strcmp(typeStr, "surface") == 0)
                {
                    geometryType = GeometryType::TRIANGLES;

                    if(m_sceneDescription.m_meshes.find(geometry_id) == 
                        m_sceneDescription.m_meshes.end())
                    {
                        globalLogger().error() << "Mesh id \"" << geometry_id << 
                            "\" is not linked to a mesh!";
                        exit(EXIT_FAILURE);
                    }
                }
                else if (strcmp(typeStr, "volume") == 0)
                {
                    geometryType = GeometryType::AABB;
                }
                else
                {
                    globalLogger().error() << "Unknown instance type: " << typeStr;
                    exit(EXIT_FAILURE);
                }

                Instance instance;
                instance.m_geometryReference.m_type = geometryType;
                instance.m_geometryReference.m_geometryIndex = geometry_id;

                XMMATRIX S = XMMatrixScaling(scale.x, scale.y, scale.z);
                XMMATRIX R = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
                XMMATRIX T = XMMatrixTranslation(translation.x, translation.y, translation.z);

                instance.m_transform = S * R * T;

                m_sceneDescription.m_instances.insert({ LogicalIndex(id), std::move(instance) });
            }
        }
        else
        {
            globalLogger().error() << "No <instances> element found!";
            exit(EXIT_FAILURE);
        }
    }

    void XmlReader::loadTextures()
    {
        m_sceneDescription.m_textures.resize(m_texturePathFiles.size());

        #pragma omp parallel
        {
            CoInitializeEx(nullptr, COINIT_MULTITHREADED);

            #pragma omp for schedule(dynamic)
            for(int i = 0; i < m_texturePathFiles.size(); i++)
            {
                Texture texture;
                texture.initialize(m_texturePathFiles[i]);

                m_sceneDescription.m_textures[i] = std::move(texture);

                globalLogger().debug() << "Texture[" << i << "] loaded: " << m_texturePathFiles[i];
            }

            CoUninitialize();
        }
    }

    void XmlReader::loadRenderConfig(tinyxml2::XMLElement& root)
    {
        tinyxml2::XMLElement* renderConfig = root.FirstChildElement("render_config");

        if(renderConfig)
        {
            const char* applicationStr = renderConfig->Attribute("type");

            if (!applicationStr)
            {
                globalLogger().error() << "Missing 'type' attribute in <render_config> element!";
                exit(EXIT_FAILURE);
            }

            if (strcmp(applicationStr, "fge") == 0 || strcmp(applicationStr, "FGE") == 0)
            {
                m_sceneDescription.m_fgeApplicationType = FgeApplicationType::FGE;
                RenderConfig fgeRenderConfig;

                const char* integratorChar = renderConfig->Attribute("integrator");

                if(!integratorChar) integratorChar = "phong";

                string integratorStr = integratorChar;

                if(integratorStr == "phong")
                {
                    fgeRenderConfig.m_integratorType = IntegratorType::PHONG;
                }
                else if(integratorStr == "pathtracer")
                {
                    fgeRenderConfig.m_integratorType = IntegratorType::PATHTRACER;
                }
                else
                {
                    globalLogger().error() 
                        << "Unknown 'integrator' attribute in <render_config> element!";
                    exit(EXIT_FAILURE);
                }

                uint32_t spp = 1;
                renderConfig->QueryUnsignedAttribute("spp", &spp);
                fgeRenderConfig.m_spp = spp;

                uint32_t nbComputeFrames = 3;
                renderConfig->QueryUnsignedAttribute("compute_frames", &nbComputeFrames);
                fgeRenderConfig.m_nbComputeFrames = nbComputeFrames;

                uint32_t nbRenderFrames = 3;
                renderConfig->QueryUnsignedAttribute("render_frames", &nbRenderFrames);
                fgeRenderConfig.m_nbRenderFrames = nbRenderFrames;

                m_sceneDescription.m_renderConfig = fgeRenderConfig;
            }
            else if (strcmp(applicationStr, "anari") == 0 || strcmp(applicationStr, "ANARI") == 0)
            {
                m_sceneDescription.m_fgeApplicationType = FgeApplicationType::ANARI;

                const char* libraryStr = renderConfig->Attribute("library");
                const char* algorithmStr = renderConfig->Attribute("algorithm");

                if (!libraryStr || !algorithmStr)
                {
                    globalLogger().error() 
                        << "Missing 'library' or 'algorithm' attribute in <render_config> element!";
                    exit(EXIT_FAILURE);
                }

                XMFLOAT3 background = loadVector3D(*renderConfig, "background");
                float ambiantRadiance = 0;
                renderConfig->QueryFloatAttribute("ambient_radiance", &ambiantRadiance);

                fgewa::AnariRenderConfig anariConfig;
                anariConfig.m_libraryName = libraryStr;
                anariConfig.m_renderAlgorithm = algorithmStr;
                anariConfig.m_backgroundColor = background;
                anariConfig.m_ambientRadiance = ambiantRadiance;

                m_sceneDescription.m_renderConfig = anariConfig;
            }
            else
            {
                globalLogger().error() << "Unknown application type: '" << applicationStr
                    << "'. Supported values are 'fge' or 'anari'.";
                exit(EXIT_FAILURE);
            }

            globalLogger().debug() << "Application type set to: " << applicationStr;
        }
        else
        {
            globalLogger().error() << "No <render_config> element found!";
            exit(EXIT_FAILURE);
        }
    }

    XMFLOAT3 XmlReader::loadVector3D(tinyxml2::XMLElement& root, const string& name)
    {
        tinyxml2::XMLElement* pointElement = root.FirstChildElement(name.c_str());

        if (pointElement)
        {
            XMFLOAT3 point{};
            pointElement->QueryFloatAttribute("x", &point.x);
            pointElement->QueryFloatAttribute("y", &point.y);
            pointElement->QueryFloatAttribute("z", &point.z);

            return point;
        }
        else
        {
            globalLogger().error() << "Missing vector element: " << name;
            exit(EXIT_FAILURE);
            return {};
        }
    }

    XMFLOAT3 XmlReader::loadColorRGB(tinyxml2::XMLElement& root, const string& name)
    {
        tinyxml2::XMLElement* pointElement = root.FirstChildElement(name.c_str());

        if (pointElement)
        {
            XMFLOAT3 point{};
            pointElement->QueryFloatAttribute("r", &point.x);
            pointElement->QueryFloatAttribute("g", &point.y);
            pointElement->QueryFloatAttribute("b", &point.z);

            return point;
        }
        else
        {
            globalLogger().error() << "Missing color element: " << name;
            exit(EXIT_FAILURE);
            return {};
        }
    }

    DXGI_FORMAT XmlReader::loadFormat(tinyxml2::XMLElement &root, const string &name)
    {
        const char* formatStr = root.Attribute(name.c_str());

        if (!formatStr)
        {
            globalLogger().error() << "Missing format attribute: " << name;
            exit(EXIT_FAILURE);
        }

        if (strcmp(formatStr, "16_bits_integers") == 0)
            return DXGI_FORMAT_R16_UNORM;

        if (strcmp(formatStr, "8_bits_integers") == 0)
            return DXGI_FORMAT_R8_UNORM;

        if (strcmp(formatStr, "16_bits_signed") == 0)
            return DXGI_FORMAT_R16_SNORM;

        if (strcmp(formatStr, "32_bits_float") == 0)
            return DXGI_FORMAT_R32_FLOAT;

        globalLogger().error()
            << "Unsupported volume format \"" << formatStr
            << "\" for attribute \"" << name << "\"";
        exit(EXIT_FAILURE);

        return DXGI_FORMAT_UNKNOWN;
    }

    bool XmlReader::loadBool(tinyxml2::XMLElement &root, const string &name)
    {
        const char* valueStr = root.Attribute(name.c_str());

        if (!valueStr)
        {
            globalLogger().error() << "Missing boolean attribute: " << name;
            exit(EXIT_FAILURE);
        }

        bool value = false;
        if (root.QueryBoolAttribute(name.c_str(), &value) == tinyxml2::XML_SUCCESS)
            return value;

        if (strcmp(valueStr, "true") == 0 || strcmp(valueStr, "1") == 0)
            return true;

        if (strcmp(valueStr, "false") == 0 || strcmp(valueStr, "0") == 0)
            return false;

        globalLogger().error()
            << "Invalid boolean value \"" << valueStr
            << "\" for attribute \"" << name << "\"";
        exit(EXIT_FAILURE);

        return false; 
    }
}