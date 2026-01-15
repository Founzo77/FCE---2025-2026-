#pragma once

#include "ScriptComponent.hpp"

#include <fge/render/indices.hpp>

using fge::LogicalIndex;

namespace fce
{
    struct ThirdViewPlayerComponent : public ScriptComponent
    {
    public:
        LogicalIndex m_playerObjectIndex;
        LogicalIndex m_cameraIndex;
        float m_mouseSensitivity;
        float m_minDistance;
        float m_maxDistance;
        float m_heightOffset;
        float m_sideOffset;
        float m_distance;
        float m_yaw;
        float m_pitch;

        ThirdViewPlayerComponent() = default;
        ThirdViewPlayerComponent(const LogicalIndex playerObjectIndex, 
            const LogicalIndex cameraIndex, const float mouseSensitivity, const float minDistance,
            const float maxDistance, const float heightOffset, const float sideOffset) : 
            m_playerObjectIndex(playerObjectIndex), m_cameraIndex(cameraIndex), m_mouseSensitivity(mouseSensitivity),
            m_minDistance(minDistance), m_maxDistance(maxDistance), m_heightOffset(heightOffset),
            m_sideOffset(sideOffset), m_distance(minDistance), m_yaw(0), m_pitch(0.1f) {}
        virtual ~ThirdViewPlayerComponent() = default;

        ThirdViewPlayerComponent(const ThirdViewPlayerComponent&) = default;
        ThirdViewPlayerComponent(ThirdViewPlayerComponent&&) = default;

        ThirdViewPlayerComponent& operator=(const ThirdViewPlayerComponent&) = default;
        ThirdViewPlayerComponent& operator=(ThirdViewPlayerComponent&&) = default;

        void update();
    };
}