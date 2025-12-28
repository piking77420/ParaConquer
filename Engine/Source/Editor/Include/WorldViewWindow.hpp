#pragma once


#include "EditorHeader.hpp"
#include "EditorWindow.hpp"
#include "LowRenderer/DescriptorSet.hpp"
#include "Rendering/Camera.hpp"
#include "Rendering/Gbuffers.hpp"
#include "Rendering/Sampler.hpp"
#include "Resources/Texture2d.hpp"
#include "Rendering/RenderGraph.hpp"

#include <vulkan/vulkan.h>

namespace PC_CORE
{
    class RhiTexture2D;
}

namespace PC_CORE::Rendering
{
    class CameraView;
}

// TODO MAKE A VIEW CLASS

BEGIN_EDITOR_PCCORE
    class WorldViewWindow : public EditorWindow
    {
    public:
        PC_CORE::Camera camera;

        explicit WorldViewWindow(Editor& _editor, const std::string& _name);

        ~WorldViewWindow() override;

        void Update() override;

        void Render(PC_CORE::CommandList* _Cmd) override;

    protected:
        PC_CORE::Rendering::RenderGraph m_RenderGraph;

        std::unique_ptr<PC_CORE::Rendering::CameraView> m_View;

    private:
        void UpdateImguiViewPort();

        void UpdateViewPort(Tbx::Vector2i _Size);

        PC_CORE::Sampler m_ViewPortSampler;

        std::array<VkDescriptorSet, MaxFramesInFlight> imguiDescriptorSet;
    };

END_EDITOR_PCCORE
