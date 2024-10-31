#include "resources/resource_manager.hpp"

#include "stb_image.h"
#include "..\..\include\resources\shader_source.hpp"
#include "resources/mesh.hpp"
#include "resources/texture.hpp"

using namespace PC_CORE;

void ResourceManager::InitPath()
{
    //VulkanShaderCompiler::GetEnvironementVariable();

    /*
    ResourceManager::CreateAndLoad<ShaderSource>("assets/shaders/shader_base.frag");
    ResourceManager::CreateAndLoad<ShaderSource>("assets/shaders/shader_base.vert");

    ResourceManager::CreateAndLoad<ShaderSource>("assets/shaders/gizmo.frag");
    ResourceManager::CreateAndLoad<ShaderSource>("assets/shaders/gizmo.vert");

    ResourceManager::CreateAndLoad<ShaderSource>("assets/shaders/skybox.frag");
    ResourceManager::CreateAndLoad<ShaderSource>("assets/shaders/skybox.vert");

    ResourceManager::CreateAndLoad<ShaderSource>("assets/shaders/draw_texture.frag");
    ResourceManager::CreateAndLoad<ShaderSource>("assets/shaders/draw_texture.vert");

    ResourceManager::CreateAndLoad<ShaderSource>("assets/shaders/skybox_procedural.frag");
    ResourceManager::CreateAndLoad<ShaderSource>("assets/shaders/skybox_procedural.vert");*/
    
    ResourceManager::Create<ShaderSource>("assets/shaders/main.vert");
    ResourceManager::Create<ShaderSource>("assets/shaders/main.frag");

    //ResourceManager::Create<Mesh>("assets/meshs/obj/rounded_cube.obj");
    ResourceManager::Create<Mesh>("assets/meshs/obj/quad.obj");
    ResourceManager::Create<Mesh>("assets/meshs/obj/sphere.obj");
    ResourceManager::Create<Mesh>("assets/meshs/obj/viking_room.obj");
    ResourceManager::Create<Mesh>("assets/meshs/obj/cube.obj");

    ResourceManager::Create<Texture>("assets/textures/diamond_block.jpg");
    /*
    ResourceManager::Create<Texture>("assets/textures/viking_room.png");
    ResourceManager::Create<Texture>("assets/textures/skydom.png");*/

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


