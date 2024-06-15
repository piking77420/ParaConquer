#include "world/skybox.hpp"

#include "../rendering/skybox_render.h"
#include "rendering/vulkan/vulkan_texture.hpp"

using namespace PC_CORE;

void Skybox::Load(std::array<std::string, 6>& _maps)
{
    texture.Load(_maps);
    SkyboxRender::UpdateDescritptorSet()
}
