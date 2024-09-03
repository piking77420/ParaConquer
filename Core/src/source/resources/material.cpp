#include "resources/material.hpp"

#include "rendering/vulkan/vulkan_instance.hpp"
#include "rendering/vulkan/vulkan_interface.hpp"
#include "rendering/vulkan/vulkan_material_manager.hpp"

using namespace PC_CORE;

void Material::WriteFile(const fs::path& path)
{
    IResource::WriteFile(path);
}

Material::~Material()
{
   
}

void Material::Load(const fs::path& path)
{
    throw std::runtime_error("error");
}

void Material::Load(std::vector<Texture*> textures)
{
    albedo = textures[0];
    VulkanInterface::vulkanMaterialManager.CreateMaterial(*this);
}
