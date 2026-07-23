#pragma once

#include <array>

#include "GpuBuffer.hpp"

BEGIN_PCCORE
    class PC_CORE_API UniformBuffer final : public GpuBuffer
    {
    public:

        DEFAULT_COPY_MOVE_OPERATIONS(UniformBuffer);

        explicit UniformBuffer(PC_CORE::Rhi& rhi, const std::string& _name, size_t _size);
    
        explicit UniformBuffer() = default;

        ~UniformBuffer() override = default;

    private:
    };

END_PCCORE
