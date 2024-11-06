#include "resources/material.hpp"


using namespace PC_CORE;



void Material::Build()
{
    
}

void Material::WriteFile(const fs::path& path)
{
    Resource::WriteFile(path);
}

Material::Material(const fs::path& _path) : Resource(_path)
{
}

Material::~Material()
{
   
}



void Material::Load(std::vector<Texture*> textures)
{
    albedo = textures[0];
   // VulkanInterface::vulkanMaterialManager.CreateMaterial(*this);
}
