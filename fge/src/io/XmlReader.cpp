#include <fge/io/XmlReader.hpp>
#include <fge/io/ObjReader.hpp>
#include <fge/io/GlobalLogger.hpp>

#include <iostream>

namespace fge
{
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

        loadCamera(*root);
        loadEmitters(*root);
        loadMeshes(*root);
        loadInstances(*root);

        globalLogger().debug() << "Scene successfully loaded: " << pathFile;
    }

    fge::XmlReader::XmlReader(tinyxml2::XMLElement& root)
    {
        loadCamera(root);
        loadEmitters(root);
        loadMeshes(root);
        loadInstances(root);

        globalLogger().debug() << "Render scene successfully loaded";
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

            vector<string> texturePathFiles;
            unordered_map<string, uint32_t> nameMaterialToTextureIndex;
            unordered_map<string, uint32_t> pathTextureToTextureIndex;

            for (tinyxml2::XMLElement* includeFile = 
                objectsElement->FirstChildElement("include"); 
                includeFile != nullptr;
                includeFile = includeFile->NextSiblingElement("include")) 
            {

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
                    mesh, m_sceneDescription.m_materials, texturePathFiles, nameMaterialToTextureIndex,
                    pathTextureToTextureIndex, nameMaterialToMaterialIndex);
                m_sceneDescription.m_meshes.insert({ LogicalIndex(id), std::move(mesh) });
            }

            for(int i = 0; i < texturePathFiles.size(); i++)
            {
                Texture texture;
                texture.initialize(texturePathFiles[i]);
                globalLogger().debug() << "Texture[" << i << "] loaded: " << texturePathFiles[i];
                m_sceneDescription.m_textures.push_back(std::move(texture));
            }
        }
        else
        {
            globalLogger().error() << "No <meshes> element found!";
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

                int mesh_id = 0;
                includeFile->QueryIntAttribute("mesh_id", &mesh_id);
                int id = 0;
                includeFile->QueryIntAttribute("id", &id);

                if(m_sceneDescription.m_meshes.find(mesh_id) == m_sceneDescription.m_meshes.end())
                {
                    globalLogger().error() << "Mesh id \"" << mesh_id << "\" is not linked to a mesh!";
                    exit(EXIT_FAILURE);
                }

                Instance instance;
                instance.m_meshIndex = mesh_id;

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
}