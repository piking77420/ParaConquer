#pragma once

#include <functional>
#include <map>


#include "core_header.hpp"
#include "guid.hpp"
#include "log.hpp"
#include "resource.hpp"

BEGIN_PCCORE



template <typename ResourceDerived>
using ResourceHandle = std::shared_ptr<ResourceDerived>;

// TODO AVOID STATIC VARIABLE
class ResourceManager
{
public:
    ResourceManager() = delete;

    ~ResourceManager() = delete;

    PC_CORE_API static void InitPath();

    PC_CORE_API static void Destroy();

    template<class ResourceDerived>
    static std::shared_ptr<ResourceDerived>  Create(const fs::path& path);
    
    template<class ResourceDerived, typename... Arg>
    static std::shared_ptr<ResourceDerived>  Create(Arg... args);
    
    template<class ResourceDerived>
    static std::shared_ptr<ResourceDerived> Get(const std::string& _name);

    template<class ResourceDerived>
    static bool Exist(const std::string& _name);

    template<class ResourceDerived>
    static std::shared_ptr<ResourceDerived> Get();

    PC_CORE_API static std::shared_ptr<Resource> GetByGuid(const Guid& _guid);

    template<class ResourceDerived>
    static bool Delete(const std::string& _name);

    template <class ResourceDerived>
    static void ForEach(const std::function<void(ResourceDerived*)>& _lamba);
    
    PC_CORE_API static void ForEach(TypeId typeID, const std::function<void(std::shared_ptr<Resource>)>& _lamba);

    PC_CORE_API static void LinkDepencies(const std::string& _parentResource, const std::string& _childResource);

    PC_CORE_API void ReloadResourceDepencencies(const std::string& _parentResource);

    PC_CORE_API static const std::vector<std::weak_ptr<Resource>>* GetChildResource(const std::string& _resourceName);    
    
private:
    PC_CORE_API static inline std::unordered_map<std::string, std::shared_ptr<Resource>> m_ResourcesMap;

    // Maps a resource to the resources that depend on it (its parents)
    PC_CORE_API static inline std::unordered_map<std::string, std::vector<std::weak_ptr<Resource>>> m_ResourceToParents;

    // Maps a resource to the resources it depends on (its children)
    PC_CORE_API static inline std::unordered_map<std::string, std::vector<std::weak_ptr<Resource>>> m_ResourceToChildren;
    
    PC_CORE_API static void SerializeResource();

    PC_CORE_API static void DeserializeResource();


    
    REFLECT(std::shared_ptr<Resource>)
    REFLECT(std::unordered_map<std::string, std::shared_ptr<Resource>>)
    REFLECT(std::unordered_map<Guid, std::vector<std::weak_ptr<Resource>>>)
    
};




template <class ResourceDerived>
std::shared_ptr<ResourceDerived> ResourceManager::Create(const fs::path& path)
{
    
    std::shared_ptr<Resource> newR = std::make_shared<ResourceDerived>(path);
   
    m_ResourcesMap.insert({ newR->name, newR });

    return std::reinterpret_pointer_cast<ResourceDerived>(newR);
}


template<class ResourceDerived, typename... Arg>
std::shared_ptr<ResourceDerived> ResourceManager::Create(Arg... args)
{
    std::shared_ptr<ResourceDerived> newR = std::make_shared<ResourceDerived>(std::forward<Arg>(args)...);
    m_ResourcesMap.insert({ newR->name, newR });

    return newR;
}



template <class ResourceDerived>
std::shared_ptr<ResourceDerived> ResourceManager::Get(const std::string& _name)
{
    auto it = m_ResourcesMap.find(_name);
    if (it != m_ResourcesMap.end())
    {
        return std::reinterpret_pointer_cast<ResourceDerived>(it->second);
    }

    PC_LOGERROR("There is no resource with this name " + _name);
    
   return nullptr;
}

template <class ResourceDerived>
bool ResourceManager::Exist(const std::string& _name)
{
    return m_ResourcesMap.contains(_name);
}

template <class ResourceDerived>
std::shared_ptr<ResourceDerived> ResourceManager::Get()
{
    for (auto it = m_ResourcesMap.begin(); it != m_ResourcesMap.end(); it++)
    {
        if (dynamic_cast<ResourceDerived*>(it->second))
            return it->second;
    }
    PC_LOGERROR("There is no resource as this type");

    return nullptr;
}



template <class ResourceDerived>
bool ResourceManager::Delete(const std::string& _name)
{

    for (auto it = m_ResourcesMap.begin(); it != m_ResourcesMap.end(); it++)
    {
        if (it->second->name == _name)
        {
            delete it->second;
            m_ResourcesMap.erase(it);
            return true;
        }
    }
    return false;
}

template <class ResourceDerived>
void ResourceManager::ForEach(const std::function<void(ResourceDerived*)>& _lamba)
{
    const TypeId typeId = Reflector::GetTypeKey<ResourceDerived>();
    
    for (auto it = m_ResourcesMap.begin(); it != m_ResourcesMap.end(); it++)
    {
        const ResourceDerived* interface = reinterpret_cast<ResourceDerived*>(it->second);
        
        if (typeId != interface->GetType().typeId)
            continue;
        
        _lamba(reinterpret_cast<ResourceDerived*>(it->second));
    }
}




END_PCCORE
