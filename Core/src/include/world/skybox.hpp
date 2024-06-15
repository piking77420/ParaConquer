#pragma once
#include <string>

#include "core_header.hpp"
#include "rendering/vulkan/vulkan_texture.hpp"
#include "resources/texture.hpp"

BEGIN_PCCORE
class Skybox
{
public:
    void Load(std::array<std::string,6>& _maps);

private:
    Texture texture;

    uint32_t desciptorId = -1;
};

END_PCCORE