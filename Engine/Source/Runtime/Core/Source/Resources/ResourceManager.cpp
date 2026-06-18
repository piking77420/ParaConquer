#include "Resources/ResourceManager.hpp"

#include "stb_image.h"
#include "Rendering/Material.hpp"
#include "Rendering/Sampler.hpp"
#include "Resources/Texture2d.hpp"

#include "Serialize/Serializer.h"

using namespace PC_CORE;

static constexpr bool CreateBasicsResource = true;

#define RESSOURCE_MAP_FILE "ParaConquerResource.res"

ResourceManager::ResourceManager()
{
    assert(m_ResourceManager == nullptr);
    m_ResourceManager = this;
}

ResourceManager::~ResourceManager()
{

}

void ResourceManager::Destroy()
{
    auto& instance = Instance();
    std::scoped_lock _(instance.m_lock);

    for (auto it = instance.m_NameToGuid.begin(); it != instance.m_NameToGuid.end(); ++it)
    {
        instance.m_ResourcesMap[it->second];
        if (instance.m_ResourcesMap[it->second].use_count() > 1)
        {
            PC_LOGERROR(
                "There is a remaining reference before destroyed by the resource manager " + instance.m_ResourcesMap[
                    it->second]->Name);
        }
        instance.m_ResourcesMap[it->second].reset();
        instance.m_ResourcesMap[it->second] = nullptr;
    }
    instance.m_NameToGuid.clear();
}

const std::string& ResourceManager::GetName(const Guid& _guid)
{
    auto& instance = Instance();
    std::scoped_lock _(instance.m_lock);

    return instance.m_ResourcesMap.at(_guid)->Name;
}

bool ResourceManager::Exist(const std::string& _name)
{
    auto& instance = Instance();
    std::scoped_lock _(instance.m_lock);

    return instance.m_NameToGuid.contains(_name);
}

bool ResourceManager::Exist(const Guid& _guid)
{
    auto& instance = Instance();
    std::scoped_lock _(instance.m_lock);


    return instance.m_ResourcesMap.contains(_guid);
}

void ResourceManager::ForEach(TypeId typeID, const std::function<void(std::shared_ptr<Resource>)>& _lamba)
{
    std::scoped_lock _(Instance().m_lock);

    if (!Reflector::Exist(typeID))
        return;
    auto& instance = Instance();
    for (auto it = instance.m_ResourcesMap.begin(); it != instance.m_ResourcesMap.end(); ++it)
    {
        if (typeID != it->second->GetType().typeId)
            continue;

        _lamba(it->second);
    }
}

size_t ResourceManager::GetResourceCount()
{
    auto& instance = Instance();
    std::scoped_lock _(instance.m_lock);

    return instance.m_ResourcesMap.size();
}

ResourceManager& ResourceManager::Instance()
{
    return *m_ResourceManager;
}


bool ResourceManager::Add(const ObjectPtr<Resource>& _object)
{
    auto& instance = Instance();
    std::scoped_lock _(instance.m_lock);


    auto& resourcesMap = instance.m_ResourcesMap;
    auto& nameToGuid = instance.m_NameToGuid;

    if (const bool guidExist = resourcesMap.contains(_object->GetGuid()))
    {
        PC_LOGERROR("There is already a resource with this guid {}", static_cast<std::string>(_object->GetGuid()));
        return false;
    }
    if (const bool nameExist = nameToGuid.contains(_object->Name))
    {
        PC_LOGERROR("There is already a resource with this Name {}", _object->Name);
        return false;
    }
    resourcesMap.insert({ _object->GetGuid(), _object });
    nameToGuid.insert({ _object->Name, _object->GetGuid() });

    return true;
}