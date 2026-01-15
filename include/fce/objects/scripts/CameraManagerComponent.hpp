#pragma once

#include "ScriptComponent.hpp"

#include <fge/render/indices.hpp>

#include <vector>

using fge::LogicalIndex;
using std::vector;

namespace fce
{
    struct CameraManagerComponent : public ScriptComponent
    {
    private:
        vector<LogicalIndex> m_cameras;
        int m_currentId;

    public:
        CameraManagerComponent() = default;
        CameraManagerComponent(const vector<LogicalIndex>& cameras) : 
            m_cameras(cameras), m_currentId(0) {}
        virtual ~CameraManagerComponent() = default;

        CameraManagerComponent(const CameraManagerComponent&) = default;
        CameraManagerComponent(CameraManagerComponent&&) = default;

        CameraManagerComponent& operator=(const CameraManagerComponent&) = default;
        CameraManagerComponent& operator=(CameraManagerComponent&&) = default;

        void update();
    };
}