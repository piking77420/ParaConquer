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
    
}

void ResourceManager::Destroy()
{
    for (auto it = Instance().m_NameToGuid.begin(); it != Instance().m_NameToGuid.end(); it++)
    {

        Instance().m_ResourcesMap[it->second];
        if (Instance().m_ResourcesMap[it->second].use_count() > 1)
        {
            PC_LOGERROR("There is a remaining reference before destroyed by the resource manager " + Instance().m_ResourcesMap[it->second]->name);
        }
        Instance().m_ResourcesMap[it->second].reset();
        Instance().m_ResourcesMap[it->second] = nullptr;
    }
    Instance().m_NameToGuid.clear();
}

const std::string& ResourceManager::GetName(const Guid& _guid)
{
    return Instance().m_ResourcesMap.at(_guid)->name;
}

bool ResourceManager::Exist(const std::string& _name)
{
    return Instance().m_NameToGuid.contains(_name);
}

bool ResourceManager::Exist(const Guid& _guid)
{
    return Instance().m_ResourcesMap.contains(_guid);
}

void ResourceManager::ForEach(TypeId typeID, const std::function<void(std::shared_ptr<Resource>)>& _lamba)
{
    if (!Reflector::Exist(typeID))
        return;
        
    for (auto it = Instance().m_ResourcesMap.begin(); it != Instance().m_ResourcesMap.end(); it++)
    {
        if (typeID != it->second->GetType().typeId)
            continue;
        
        _lamba(it->second);
    }
    
}


