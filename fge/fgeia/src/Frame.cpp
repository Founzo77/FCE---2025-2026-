#include <fgeia/Frame.hpp>
#include <fgeia/FgeGlobalState.hpp>

#include <fge/utility.hpp>

using fge::throwIfFailed;

namespace fgeia
{
    Frame::Frame(FgeGlobalState* globalState) : helium::BaseFrame(globalState), m_renderer(this)
    {

    }

    FgeGlobalState* Frame::deviceState()
    {
        return (FgeGlobalState*) helium::BaseObject::m_state;
    }

    bool Frame::isValid() const
    {
        return m_renderer && m_renderer->isValid() && m_camera && m_camera->isValid() 
            && m_world && m_world->isValid();
    }

    bool Frame::getProperty(const std::string_view& name, 
        ANARIDataType type, void *ptr, uint64_t size, uint32_t flags)
    {
        // TO_DO
        return false;
    }

    void Frame::commitParameters()
    {
        m_renderer = getParamObject<Renderer>("renderer");
        m_camera = getParamObject<Camera>("camera");
        m_world = getParamObject<World>("world");
        m_channelTypes.color = getParam<anari::DataType>("channel.color", ANARI_UNKNOWN);
        m_channelTypes.dx12Color = getParam<anari::DataType>("channel.dx12Color", ANARI_UNKNOWN);
        m_channelTypes.depth = getParam<anari::DataType>("channel.depth", ANARI_UNKNOWN);
        m_channelTypes.primID = getParam<anari::DataType>("channel.primitiveId", ANARI_UNKNOWN);
        m_channelTypes.instID = getParam<anari::DataType>("channel.instanceId", ANARI_UNKNOWN);
        m_channelTypes.objID = getParam<anari::DataType>("channel.objectId", ANARI_UNKNOWN);
        m_channelTypes.normal = getParam<anari::DataType>("channel.normal", ANARI_UNKNOWN);
        m_size = getParam<anari::math::uint2>("size", anari::math::uint2(10, 10));
        m_enableDenoising = getParam<int>("enableDenoising", 0);
    }

    void Frame::finalize()
    {
        if (!m_renderer) 
        {
            reportMessage(ANARI_SEVERITY_WARNING, "missing required parameter 'renderer' on frame");
        }

        if (!m_camera) 
        {
            reportMessage(ANARI_SEVERITY_WARNING, "missing required parameter 'camera' on frame");
        }

        if (!m_world) 
        {
            reportMessage(ANARI_SEVERITY_WARNING, "missing required parameter 'world' on frame");
        }

        // TO_DO Resize l'output
    }

    void Frame::renderFrame()
    {
        deviceState()->commitBuffer.flush();

        if (!isValid()) 
        {
            reportMessage(ANARI_SEVERITY_ERROR, "skipping render of incomplete frame object");
            return;
        }

        fge::RenderEngine& renderer = deviceState()->startRender(m_size.x, m_size.y);

        fge::FrameExecutionContext executionContext = renderer.startRender();
        
        
        if(m_channelTypes.color != ANARI_UNKNOWN)
        {
            renderer.copyOutputToHost();
        }
        
        if(m_channelTypes.dx12Color != ANARI_UNKNOWN)
        {
            renderer.copyOutputToSharedResource();
        }
        
        renderer.executeRender();
        renderer.endRender();

        renderer.synchronize();
    }

    void* Frame::map(std::string_view channel, uint32_t* width, uint32_t* height, 
        ANARIDataType* pixelType)
    {
        FgeGlobalState& globalState = *deviceState();
        fge::RenderEngine& renderEngine = globalState.getRenderEngine();

        *width = m_size.x;
        *height = m_size.y;

        const uint32_t numPixels = (*width) * (*height);

        if (channel == "channel.color")
        {
            ANARIDataType type = m_channelTypes.color;

            if (type == ANARI_UNKNOWN)
                type = ANARI_UFIXED8_VEC4;

            if (type == ANARI_UFIXED8_VEC4)
            {
                throwIfFailed(m_channelBuffers.colorU8 == nullptr, "Color buffer already mapped");

                m_channelBuffers.colorU8 = new uint32_t[numPixels];

                renderEngine.copyOutputHostToHost(m_channelBuffers.colorU8, 
                    numPixels * sizeof(uint32_t));

                *pixelType = ANARI_UFIXED8_VEC4;

                return m_channelBuffers.colorU8;
            }

            if (type == ANARI_FLOAT32_VEC4)
            {
                throwIfFailed(m_channelBuffers.colorF32 == nullptr, "Color buffer already mapped");

                std::vector<uint32_t> tmp(numPixels);

                m_channelBuffers.colorF32 = new float[numPixels * 4];

                renderEngine.copyOutputHostToHost(tmp.data(), numPixels * sizeof(uint32_t));

                for (uint32_t i = 0; i < numPixels; i++)
                {
                    uint32_t p = tmp[i];

                    uint8_t r = (p >> 0) & 0xFF;
                    uint8_t g = (p >> 8) & 0xFF;
                    uint8_t b = (p >> 16) & 0xFF;
                    uint8_t a = (p >> 24) & 0xFF;

                    m_channelBuffers.colorF32[i*4+0] = r / 255.f;
                    m_channelBuffers.colorF32[i*4+1] = g / 255.f;
                    m_channelBuffers.colorF32[i*4+2] = b / 255.f;
                    m_channelBuffers.colorF32[i*4+3] = a / 255.f;
                }

                *pixelType = ANARI_FLOAT32_VEC4;

                return m_channelBuffers.colorF32;
            }

            reportMessage(ANARI_SEVERITY_WARNING, "unsupported color buffer format");

            *pixelType = ANARI_UNKNOWN;
            return nullptr;
        }

        if (channel == "channel.dx12Color")
        {
            *pixelType = ANARI_VOID_POINTER;
            return renderEngine.getSharedOutputTextureHandle();
        }

        if (channel == "channel.depth")
        {
            reportMessage(ANARI_SEVERITY_WARNING, "depth channel not implemented");

            *pixelType = ANARI_UNKNOWN;
            return nullptr;
        }

        if (channel == "channel.primitiveId")
        {
            reportMessage(ANARI_SEVERITY_WARNING, "primitiveId channel not implemented");

            *pixelType = ANARI_UNKNOWN;
            return nullptr;
        }

        if (channel == "channel.instanceId")
        {
            reportMessage(ANARI_SEVERITY_WARNING, "instanceId channel not implemented");

            *pixelType = ANARI_UNKNOWN;
            return nullptr;
        }

        if (channel == "channel.objectId")
        {
            reportMessage(ANARI_SEVERITY_WARNING, "objectId channel not implemented");

            *pixelType = ANARI_UNKNOWN;
            return nullptr;
        }

        if (channel == "channel.normal")
        {
            reportMessage(ANARI_SEVERITY_WARNING, "normal channel not implemented");

            *pixelType = ANARI_UNKNOWN;
            return nullptr;
        }

        reportMessage(ANARI_SEVERITY_WARNING, 
            "unknown frame channel '%s'", std::string(channel).c_str());

        *pixelType = ANARI_UNKNOWN;

        return nullptr;
    }

    void Frame::unmap(std::string_view channel)
    {
        if (channel == "channel.color")
        {
            if(m_channelBuffers.colorU8)
            {
                delete[] m_channelBuffers.colorU8;
                m_channelBuffers.colorU8 = nullptr;
            }
            else if(m_channelBuffers.colorF32)
            {
                delete[] m_channelBuffers.colorF32;
                m_channelBuffers.colorF32 = nullptr;
            }
            return;
        }

        if(channel == "channel.dx12Color")
        {
            // Nothing to do
        }

        if (channel == "channel.depth")
        {
            delete[] m_channelBuffers.depth;
            m_channelBuffers.depth = nullptr;
            return;
        }

        if (channel == "channel.primitiveId")
        {
            delete[] m_channelBuffers.primID;
            m_channelBuffers.primID = nullptr;
            return;
        }

        if (channel == "channel.instanceId")
        {
            delete[] m_channelBuffers.instID;
            m_channelBuffers.instID = nullptr;
            return;
        }

        if (channel == "channel.objectId")
        {
            delete[] m_channelBuffers.objID;
            m_channelBuffers.objID = nullptr;
            return;
        }

        if (channel == "channel.normal")
        {
            delete[] m_channelBuffers.normal;
            m_channelBuffers.normal = nullptr;
            return;
        }
    }

    int Frame::frameReady(ANARIWaitMask waitMask)
    {
        if (waitMask == ANARI_NO_WAIT)
            return 1;
        else 
            return 1;
    }

    void Frame::discard()
    {
        // no-op (not yet async)
    }
}

FGE_ANARI_TYPEFOR_DEFINITION(fgeia::Frame*);
