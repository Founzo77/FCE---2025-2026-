#pragma once

#include "SceneDescription.hpp"

#include <tinyxml2.h>

namespace fge
{
    class XmlReader
    {
    public:
        SceneDescription m_sceneDescription;

    public:
        XmlReader(const string& pathFile);
        XmlReader(tinyxml2::XMLElement& root);

    private:
        void loadCamera(tinyxml2::XMLElement& root);
        void loadEmitters(tinyxml2::XMLElement& root);
        void loadMeshes(tinyxml2::XMLElement& root);
        void loadInstances(tinyxml2::XMLElement& root);

    public:
        static XMFLOAT3 loadVector3D(tinyxml2::XMLElement& root, const string& name);
        static XMFLOAT3 loadColorRGB(tinyxml2::XMLElement& root, const string& name);
    };
}