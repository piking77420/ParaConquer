#pragma once
#include <string>

#include "core_header.hpp"
#include "rendering/vulkan/vulkan_texture.hpp"
#include "resources/texture.hpp"

BEGIN_PCCORE
class Skybox
{
public:
    enum class SkyBoxType
    {
        CubeMap,
        Procedural
    };

    SkyBoxType skyboxType = SkyBoxType::CubeMap;
    
    Texture* texture = nullptr;
    
    void Load(std::array<std::string,6>& _maps);
    
    void Destroy();

    Skybox() = default;

    ~Skybox() = default;
    
};

END_PCCORE