#pragma once

#include "indices.hpp"

#include <cstdint>

namespace fge
{
    class Light;
    class Instance;
    class Camera;

    class FgeScene
    {
    public:
        virtual ~FgeScene() = default;

        virtual const Light& getLight(const LogicalIndex lightIndex) = 0;
        virtual void addLight(Light& newLight, const LogicalIndex lightIndex) = 0;
        virtual void modifyLight(Light& modifiedLight, const LogicalIndex lightIndex) = 0;
        virtual void deleteLight(const LogicalIndex lightIndex) = 0;

        virtual const Instance& getInstance(const LogicalIndex instanceIndex) = 0;
        virtual void addInstance(Instance& newInstance, const LogicalIndex instanceIndex) = 0;
        virtual void modifyInstance(Instance& newInstance, const LogicalIndex instanceIndex) = 0;
        virtual void deleteInstance(const LogicalIndex instanceIndex) = 0;

        virtual void moveCameraForward(float distance) = 0;
        virtual void moveCameraRight(float distance) = 0;
        virtual void moveCameraUp(float distance) = 0;
        virtual void rotateCameraY(float distance) = 0;
        virtual void rotateCamera(float x, float y) = 0;

        virtual void setCamera(const Camera& camera) = 0;

        virtual uint32_t getNbMaxMeshes() = 0;
        virtual uint32_t getNbMaxSubMeshes() = 0;
        virtual uint32_t getNbMaxInstances() = 0;
        virtual uint32_t getNbMaxMaterials() = 0;
        virtual uint32_t getNbMaxTextures() = 0;
        virtual uint32_t getNbMaxLights() = 0;
    };
}