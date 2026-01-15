#pragma once

#include "ScriptComponent.hpp"

#include <fge/render/indices.hpp>

#include <vector>

using fge::LogicalIndex;
using std::vector;

namespace fce
{
    struct PlayerManagerComponent : public ScriptComponent
    {
    private:
        vector<LogicalIndex> m_players;
        int m_currentId;
        float m_moveSpeed;
        float m_rotateSpeed;
        float m_acceleration;

    public:
        PlayerManagerComponent() = default;
        PlayerManagerComponent(vector<LogicalIndex> players, 
            const float moveSpeed, const float rotateSpeed,
            const float acceleration) : m_players(players), m_currentId(0),
            m_moveSpeed(moveSpeed), m_rotateSpeed(rotateSpeed), m_acceleration(acceleration) {}
        virtual ~PlayerManagerComponent() = default;

        PlayerManagerComponent(const PlayerManagerComponent&) = default;
        PlayerManagerComponent(PlayerManagerComponent&&) = default;

        PlayerManagerComponent& operator=(const PlayerManagerComponent&) = default;
        PlayerManagerComponent& operator=(PlayerManagerComponent&&) = default;

        void update();
    };
}