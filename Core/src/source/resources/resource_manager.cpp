#include "resources/resource_manager.hpp"

#include "stb_image.h"
#include "resources/mesh.hpp"
#include "resources/texture.hpp"



using namespace PC_CORE;

void ResourceManager::InitPath()
{

    ResourceManager::Create<Mesh>("assets/meshs/obj/rounded_cube.obj");
    ResourceManager::Create<Mesh>("assets/meshs/obj/quad.obj");
    ResourceManager::Create<Mesh>("assets/meshs/obj/sphere.obj");
    ResourceManager::Create<Mesh>("assets/meshs/obj/viking_room.obj");
    ResourceManager::Create<Mesh>("assets/meshs/obj/cube.obj");
    ResourceManager::Create<Mesh>("assets/meshs/obj/untitled.obj");
    ResourceManager::Create<Mesh>("assets/meshs/obj/suzanne.obj");


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


