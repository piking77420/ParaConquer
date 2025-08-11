#include "Resources/ResourceManager.hpp"

#include "stb_image.h"
#include "Rendering/Material.hpp"
#include "Rendering/Sampler.hpp"
#include "Resources/Mesh.hpp"
#include "Resources/Texture2d.hpp"

#include "Serialize/Serializer.h"

using namespace PC_CORE;

static constexpr bool CreateBasicsResource = true;

#define RESSOURCE_MAP_FILE "ParaConquerResource.res"


void ResourceManager::InitPath()
{
    PERF_REGION_SCOPED;
    auto s = ResourceManager::Create<Mesh>("RoundedCube.obj", "Assets/Meshs/Obj/RoundedCube.obj");

    auto mesh1 = ResourceManager::Create<Mesh>("Suad.obj", "Assets/Meshs/Obj/Quad.obj");

    auto mesh2 = ResourceManager::Create<Mesh>("Sphere.obj", "Assets/Meshs/Obj/Sphere.obj");

    auto mesh3 = ResourceManager::Create<Mesh>("Capsule.obj", "Assets/Meshs/Obj/Capsule.obj");

    auto mesh4 = ResourceManager::Create<Mesh>("VikingRoom.obj", "Assets/Meshs/Obj/VikingRoom.obj");

    auto mesh5 = ResourceManager::Create<Mesh>("Cube.obj", "Assets/Meshs/Obj/Cube.obj");

    auto mesh6 = ResourceManager::Create<Mesh>("Untitled.obj", "Assets/Meshs/Obj/Untitled.obj");

    auto mesh7 = ResourceManager::Create<Mesh>("Suzanne.obj", "Assets/Meshs/Obj/Suzanne.obj");

    auto texture1 = ResourceManager::Create<Texture2D>("DiamondBlock.jpg", "Assets/textures/DiamondBlock.jpg");

    auto texture2 = ResourceManager::Create<Texture2D>("EmerauldBlock.png","Assets/textures/EmerauldBlock.png");

    
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

    SamplerCreateInfo ClampToEdgeInfo =
    {
        .SamplerName = "ClampToEdge",
        .magFilter = Filter::NEAREST,
        .minFilter = Filter::NEAREST,
        .u = SamplerAddressMode::CLAMP_TO_EDGE,
        .v = SamplerAddressMode::CLAMP_TO_EDGE,
        .w = SamplerAddressMode::CLAMP_TO_EDGE
        };
    ResourceManager::Create<PC_CORE::Sampler>(ClampToEdgeInfo);
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


