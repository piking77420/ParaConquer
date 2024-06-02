#include "resources/resource_manager.hpp"

#include "..\..\include\resources\shader_source.hpp"
#include "rendering/vulkan/vulkan_shader_compiler.hpp"
#include "resources/mesh.hpp"

using namespace PC_CORE;

void ResourceManager::Init()
{
    VulkanShaderCompiler::GetEnvironementVariable();

    ResourceManager::CreateAndLoad<ShaderSource>("assets/shaders/shader_base.frag");
    ResourceManager::CreateAndLoad<ShaderSource>("assets/shaders/shader_base.vert");


    ResourceManager::CreateAndLoad<Mesh>("assets/meshs/obj/cube.obj");
    ResourceManager::CreateAndLoad<Mesh>("assets/meshs/obj/quad.obj");
    ResourceManager::CreateAndLoad<Mesh>("assets/meshs/obj/sphere.obj");
    ResourceManager::CreateAndLoad<Mesh>("assets/meshs/obj/viking_room.obj");
}

void ResourceManager::Destroy()
{
    for (auto it = m_ResourcesMap.begin(); it != m_ResourcesMap.end(); it++)
    {
        delete it->second;
        it->second = nullptr;
    }
    m_ResourcesMap.clear();
}


