#include "resources/resource_manager.hpp"

#include "stb_image.h"
#include "rendering/material.hpp"
#include "rendering/sampler.hpp"
#include "resources/mesh.hpp"
#include "resources/texture_2d.hpp"

#include "serialize/serializer.h"

using namespace PC_CORE;

static constexpr bool CreateBasicsResource = true;

#define RESSOURCE_MAP_FILE "paraconquer_resource.res"


void ResourceManager::InitPath()
{
    PERF_REGION_SCOPED;
    auto s = ResourceManager::Create<Mesh>("rounded_cube.obj", "assets/meshs/obj/rounded_cube.obj");

    auto mesh1 = ResourceManager::Create<Mesh>("quad.obj", "assets/meshs/obj/quad.obj");

    auto mesh2 = ResourceManager::Create<Mesh>("sphere.obj", "assets/meshs/obj/sphere.obj");

    auto mesh3 = ResourceManager::Create<Mesh>("capsule.obj", "assets/meshs/obj/capsule.obj");

    auto mesh4 = ResourceManager::Create<Mesh>("viking_room.obj", "assets/meshs/obj/viking_room.obj");

    auto mesh5 = ResourceManager::Create<Mesh>("cube.obj", "assets/meshs/obj/cube.obj");

    auto mesh6 = ResourceManager::Create<Mesh>("untitled.obj", "assets/meshs/obj/untitled.obj");

    auto mesh7 = ResourceManager::Create<Mesh>("suzanne.obj", "assets/meshs/obj/suzanne.obj");

    auto texture1 = ResourceManager::Create<Texture2D>("diamond_block.jpg", "assets/textures/diamond_block.jpg");

    auto texture2 = ResourceManager::Create<Texture2D>("emerauld_block.png","assets/textures/emerauld_block.png");

    
    const SamplerCreateInfo info =
        {
        .SamplerName = "LinearRepeat",
        .magFilter = Filter::LINEAR,
        .minFilter = Filter::LINEAR,
        .u = SamplerAddressMode::REPEAT,
        .v = SamplerAddressMode::REPEAT,
        .w = SamplerAddressMode::REPEAT
        };
    ResourceManager::Create<PC_CORE::Sampler>(info);
}

void ResourceManager::Destroy()
{
    SerializeResource();

    for (auto it = m_NameToGuid.begin(); it != m_NameToGuid.end(); it++)
    {

        m_ResourcesMap[it->second];
        if (m_ResourcesMap[it->second].use_count() > 1)
        {
            PC_LOGERROR("There is a remaining reference before destroyed by the resource manager " + m_ResourcesMap[it->second]->name);
        }
        m_ResourcesMap[it->second].reset();
        m_ResourcesMap[it->second] = nullptr;
    }
    m_NameToGuid.clear();
}


const std::string& ResourceManager::GetName(const Guid& _guid)
{
    return m_ResourcesMap.at(_guid)->name;
}


bool ResourceManager::Exist(const Guid& _guid)
{
    return m_ResourcesMap.contains(_guid);
}

void ResourceManager::ForEach(TypeId typeID, const std::function<void(std::shared_ptr<Resource>)>& _lamba)
{
    if (!Reflector::ContaintTypeFromTypeID(typeID))
        return;
        
    for (auto it = m_ResourcesMap.begin(); it != m_ResourcesMap.end(); it++)
    {
        if (typeID != it->second->GetType().typeId)
            continue;
        
        _lamba(it->second);
    }
    
}

void ResourceManager::SerializeResource()
{
  
}


