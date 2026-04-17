#pragma once

#include "World.hpp"
#include "Camera.hpp"
#include "Renderer.hpp"

#include <helium/BaseFrame.h>

#include <fge/PlatformWindows.hpp>
#include <wrl/client.h>
#include <d3d12.h>

using namespace Microsoft::WRL;

namespace fgeia
{
    class Frame : public helium::BaseFrame
    {
    private:
        bool m_valid {false};
        bool m_enableDenoising {true};
        anari::math::uint2 m_size { 0,0 };

        struct {
            uint32_t* colorU8{nullptr};
            float* colorF32{nullptr};
            struct {

                HANDLE dx12SharedHandle{nullptr};
            } dx12Color;

            float* depth{nullptr};
            int* primID{nullptr};
            int* instID{nullptr};
            int* objID{nullptr};
            float* normal{nullptr};
        } m_channelBuffers;

        struct {
            /* for performance warnings; initialize all to 'true' so they
                won't throw a perf warning on first time renderframe */
            bool color = true;
            bool depth = true;
            bool primID = true;
            bool instID = true;
            bool objID = true;
            bool normal = true;
        } m_didMapChannel;

        bool m_lastFrameWasFirstFrame = true;

        struct {
            anari::DataType color{ANARI_UNKNOWN};
            anari::DataType dx12Color{ANARI_UNKNOWN};
            anari::DataType depth{ANARI_UNKNOWN};
            anari::DataType primID{ANARI_UNKNOWN};
            anari::DataType instID{ANARI_UNKNOWN};
            anari::DataType objID{ANARI_UNKNOWN};
            anari::DataType normal{ANARI_UNKNOWN};
        } m_channelTypes;

        helium::ChangeObserverPtr<Renderer> m_renderer;
        helium::IntrusivePtr<Camera> m_camera;
        helium::IntrusivePtr<World> m_world;

        helium::TimeStamp m_lastCommitFlush{0};

    public:
        Frame(FgeGlobalState* globalState);
        virtual ~Frame() override = default;

        FgeGlobalState* deviceState();

        bool isValid() const override;
        bool getProperty(const std::string_view& name, ANARIDataType type,
            void* ptr, uint64_t size, uint32_t flags) override;

        void commitParameters() override;
        void finalize() override;
        void renderFrame() override;

        void* map(std::string_view channel, uint32_t* width, uint32_t* height,
            ANARIDataType* pixelType) override;
        void unmap(std::string_view channel) override;
        int frameReady(ANARIWaitMask waitMask) override;
        void discard() override;
    };
}

FGE_ANARI_TYPEFOR_SPECIALIZATION(fgeia::Frame*, ANARI_FRAME);
