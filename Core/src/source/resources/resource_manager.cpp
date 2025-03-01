#include "resources/resource_manager.hpp"

#include "stb_image.h"
#include "rendering/material.hpp"
#include "resources/mesh.hpp"
#include "resources/texture.hpp"

#include "serialize/serializer.h"

using namespace PC_CORE;

void ResourceManager::InitPath()
{

    ResourceManager::Create<Mesh>("assets/meshs/obj/rounded_cube.obj");
    ResourceManager::Create<Mesh>("assets/meshs/obj/quad.obj");
    auto it = ResourceManager::Create<Mesh>("assets/meshs/obj/sphere.obj");

    ResourceManager::Create<Mesh>("assets/meshs/obj/viking_room.obj");
    ResourceManager::Create<Mesh>("assets/meshs/obj/cube.obj");
    ResourceManager::Create<Mesh>("assets/meshs/obj/untitled.obj");
    ResourceManager::Create<Mesh>("assets/meshs/obj/suzanne.obj");


    auto it2 = ResourceManager::Create<Texture>("assets/textures/diamond_block.jpg");

    ResourceManager::Create<Texture>("assets/textures/emerauld_block.png");

    /*
    ResourceManager::Create<Texture>("assets/textures/viking_room.png");
    ResourceManager::Create<Texture>("assets/textures/skydom.png");*/

}

void ResourceManager::Destroy()
{
    SerializeResource();

    for (auto it = m_ResourcesMap.begin(); it != m_ResourcesMap.end(); it++)
    {

        if (it->second.use_count() > 1)
        {
            PC_LOGERROR("There is a remaining reference before destroyed by the resource manager " + it->second->name);
        }
        it->second.reset();
        it->second = nullptr;
    }
    m_ResourcesMap.clear();
}

std::shared_ptr<Resource> ResourceManager::GetByGuid(const Guid& _guid)
{
    for (auto it = m_ResourcesMap.begin(); it != m_ResourcesMap.end(); it++)
    {
        if (it->second->guid == _guid)
            return it->second;
    }

    return nullptr;
}

void ResourceManager::ForEach(TypeId typeID, const std::function<void(std::shared_ptr<Resource>)>& _lamba)
{
    if (!Reflector::ContaintTypeFromTypeID(typeID))
        return;
        
    for (auto it = m_ResourcesMap.begin(); it != m_ResourcesMap.end(); it++)
    {
        const std::shared_ptr<const ResourceInterface<Material>> interface = std::reinterpret_pointer_cast<const ResourceInterface<Material>>(it->second);
        
        if (typeID != interface->GetType().typeId)
            continue;
        
        _lamba(it->second);
    }
    
}

void ResourceManager::SerializeResource()
{
    std::ofstream myfile(ResourceMapFile);

    Mesh* v = Get<Mesh>("suzanne").get();
    Serializer::Serialize(*v, ResourceMapFile);
}


