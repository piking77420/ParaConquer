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
	ResourceManager::Create<Mesh>((fs::path)"assets/meshs/obj/rounded_cube.obj");

	ResourceManager::Create<Mesh>((fs::path)"assets/meshs/obj/quad.obj");
	auto it = ResourceManager::Create<Mesh>((fs::path)"assets/meshs/obj/sphere.obj");

    ResourceManager::Create<Mesh>((fs::path)"assets/meshs/obj/capsule.obj");
	ResourceManager::Create<Mesh>((fs::path)"assets/meshs/obj/viking_room.obj");
	ResourceManager::Create<Mesh>((fs::path)"assets/meshs/obj/cube.obj");
	ResourceManager::Create<Mesh>((fs::path)"assets/meshs/obj/untitled.obj");
	ResourceManager::Create<Mesh>((fs::path)"assets/meshs/obj/suzanne.obj");

    auto it2 = ResourceManager::Create<Texture2D>((fs::path)"assets/textures/diamond_block.jpg");

	ResourceManager::Create<Texture2D>((fs::path)"assets/textures/emerauld_block.png");


    
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

void ResourceManager::LinkDepencies(const std::string& _parentResource, const std::string& _childResource)
{
    /*
    assert(!_parentResource.empty() && !_childResource.empty() && "Resource are null");

    
    if (_parentResource == _childResource)
    {
        PC_LOGERROR("Try to link dependencies between itself, {}", _parentResource)
    }
    
    auto itp = m_ResourcesMap.find(_parentResource);
    if (itp == m_ResourcesMap.end())
    {
        PC_LOGERROR("There is no resource name as {}", _parentResource);
        return;
    }
    auto itc = m_ResourcesMap.find(_childResource);
    if (itc == m_ResourcesMap.end())
    {
        PC_LOGERROR("There is no resource name as {}", _childResource);
        return;
    }

    // TODO FIND IF CIRCULAR DEPENDICIES

    // Compute parent
    m_ResourceToChildren[_parentResource].push_back(itc->second);
    m_ResourceToParents[_childResource].push_back(itp->second);*/
}

void ResourceManager::ReloadResourceDepencencies(const std::string& _parentResource)
{
    /*
    auto it = m_ResourceToChildren.find(_parentResource);
    if (it == m_ResourceToChildren.end())
        return;

    for (auto itc = it->second.begin(); itc != it->second.end(); itc++)
    {
           if (auto resource = itc->lock())
               resource->BroadCastReload();
           
    }*/
}


void ResourceManager::SerializeResource()
{
  
}


