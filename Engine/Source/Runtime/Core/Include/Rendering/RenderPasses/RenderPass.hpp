#pragma once

#include "CoreHeader.hpp"
#include "LowRenderer/RhiRenderPass.hpp"
#include "Rendering/Renderer.hpp"

BEGIN_PCCORE
    class RenderPass : public IGpuResource
    {
    public:
        PC_CORE_API RhiObject* const GetRhiHandle() const
        {
            return m_RhiRenderPass.get();
        }

        PC_CORE_API RenderPass();

        PC_CORE_API ~RenderPass() override = default;

    protected:
        std::shared_ptr<RhiRenderPass> m_RhiRenderPass;
    };

END_PCCORE
