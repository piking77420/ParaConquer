#include "resources/material.hpp"


using namespace PC_CORE;

void Material::WriteFile(const fs::path& path)
{
    IResource::WriteFile(path);
}

Material::~Material()
{
   
}

void Material::SetPath(const fs::path& path)
{
    throw std::runtime_error("error");
}

void Material::Load(std::vector<Texture*> textures)
{
    albedo = textures[0];
   // VulkanInterface::vulkanMaterialManager.CreateMaterial(*this);
}
