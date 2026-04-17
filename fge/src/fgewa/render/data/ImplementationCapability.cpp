#include <fgewa/render/data/ImplementationCapability.hpp>

namespace fgewa
{
    ImplementationCapability::ImplementationCapability(
        const bool hasOutputCudaBuffer, const bool hasOutputDiretX12Buffer) :
        m_hasOutputCudaBuffer(hasOutputCudaBuffer), 
        m_hasOutputDirectX12Buffer(hasOutputDiretX12Buffer)
    {

    }

    ImplementationCapability ImplementationCapability::fromLibraryName(const string& name)
    {
        // TO_DO faire avec le systeme introspection sur les extensions
        
        if (name == "visrtx")
        {
            return ImplementationCapability(true, false);
        }
        else if (name == "barney")
        {
            return ImplementationCapability(true, false);
        }
        else if (name == "visionaray_cuda")
        {
            return ImplementationCapability(true, false);
        }
        else if (name == "fgeia")
        {
            return ImplementationCapability(false, true);
        }

        return ImplementationCapability(false, false);
    }

    bool ImplementationCapability::hasOutputCudaBuffer() const noexcept
    {
        return m_hasOutputCudaBuffer;
    }

    bool ImplementationCapability::hasOutputDirectX12Buffer() const noexcept
    {
        return m_hasOutputDirectX12Buffer;
    }
}