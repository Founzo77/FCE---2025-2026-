#pragma once

#include <string>

using std::string;

namespace fgewa
{
    class ImplementationCapability
    {
    private:
        bool m_hasOutputCudaBuffer = false;
        bool m_hasOutputDirectX12Buffer = false;

    public:
        ImplementationCapability() = default;
        ~ImplementationCapability() = default;

        ImplementationCapability(const bool hasOutputCudaBuffer, const bool hasOutputDiretX12Buffer);
        static ImplementationCapability fromLibraryName(const string& name);

        ImplementationCapability(const ImplementationCapability&) = default;
        ImplementationCapability& operator=(const ImplementationCapability&) = default;
        ImplementationCapability(ImplementationCapability&& other) = default;
        ImplementationCapability& operator=(ImplementationCapability&& other) = default;

        bool hasOutputCudaBuffer() const noexcept;
        bool hasOutputDirectX12Buffer() const noexcept;
    };
}
