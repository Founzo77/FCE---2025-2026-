#pragma once

#include <oneapi/tbb/concurrent_vector.h>
#include <oneapi/tbb/concurrent_unordered_map.h>

#include "SceneDescription.hpp"

#include <tinyxml2.h>

using oneapi::tbb::concurrent_vector;
using oneapi::tbb::concurrent_unordered_map;

namespace fge
{
    class XmlReader
    {
    public:
        SceneDescription m_sceneDescription;
        concurrent_vector<string> m_texturePathFiles;
        concurrent_unordered_map<string, uint32_t> m_pathTextureToTextureIndex;

    public:
        XmlReader(const string& pathFile);
        XmlReader(tinyxml2::XMLElement& root);

    private:
        void loadAll(tinyxml2::XMLElement& root);
        void loadBackground(tinyxml2::XMLElement& root);
        void loadCamera(tinyxml2::XMLElement& root);
        void loadEmitters(tinyxml2::XMLElement& root);
        void loadMeshes(tinyxml2::XMLElement& root);
        void loadVolumes(tinyxml2::XMLElement& root);
        void loadTextures();
        void loadInstances(tinyxml2::XMLElement& root);
        void loadRenderConfig(tinyxml2::XMLElement& root);

    public:
        static XMFLOAT3 loadVector3D(tinyxml2::XMLElement& root, const string& name);
        static XMFLOAT3 loadColorRGB(tinyxml2::XMLElement& root, const string& name);
        static DXGI_FORMAT loadFormat(tinyxml2::XMLElement& root, const string& name);
        static bool loadBool(tinyxml2::XMLElement& root, const string& name);
    };
}