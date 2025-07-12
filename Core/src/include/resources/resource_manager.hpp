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

    template<class ResourceDerived, typename... Arg>
    static std::shared_ptr<ResourceDerived>  Create(Arg... args);
    
    template<class ResourceDerived>
    static std::shared_ptr<ResourceDerived> Get(const std::string& _name);

    template<class ResourceDerived>
    static std::shared_ptr<ResourceDerived> Get(const Guid& _guid);

    template<class ResourceDerived>
    static bool TryGetAs(const Guid& _guid, std::shared_ptr<ResourceDerived>* _outPtr);

    PC_CORE_API static const std::string& GetName(const Guid& _guid); 
    
    template<class ResourceDerived>
    static bool Exist(const std::string& _name);

    
    PC_CORE_API static bool Exist(const Guid& _guid);

    template<class ResourceDerived>
    static std::shared_ptr<ResourceDerived> Get();
    
    template<class ResourceDerived>
    static bool Delete(const std::string& _name);

    template <class ResourceDerived>
    static void ForEach(const std::function<void(ResourceDerived*)>& _lamba);
    
    PC_CORE_API static void ForEach(TypeId typeID, const std::function<void(std::shared_ptr<Resource>)>& _lamba);


private:
    PC_CORE_API static inline std::unordered_map<Guid, std::shared_ptr<Resource>> m_ResourcesMap;

    PC_CORE_API static inline std::unordered_map<std::string, Guid> m_NameToGuid;

    PC_CORE_API static void SerializeResource();

    PC_CORE_API static void DeserializeResource();

    
    REFLECT(std::shared_ptr<Resource>)
};



template<class ResourceDerived, typename... Arg>
std::shared_ptr<ResourceDerived> ResourceManager::Create(Arg... args)
{

    std::shared_ptr<ResourceDerived> newR = std::make_shared<ResourceDerived>(std::forward<Arg>(args)...);
    
    auto& resourcesMap = m_ResourcesMap;
    resourcesMap.insert({newR->GetGuid(), newR});
    auto& nameToGuid = m_NameToGuid;

    assert(!nameToGuid.contains(newR->name) && "There is a resource with the same name already");

    nameToGuid.insert({ newR->name, newR->GetGuid() });

    return std::reinterpret_pointer_cast<ResourceDerived>(newR);
}



template <class ResourceDerived>
std::shared_ptr<ResourceDerived> ResourceManager::Get(const std::string& _name)
{
    auto it = m_NameToGuid.find(_name);
    if (it != m_NameToGuid.end())
    {
        auto r = m_ResourcesMap.at(it->second);
        assert(r->GetGuid() == it->second);
        assert(r->name ==  _name);

        
        return std::reinterpret_pointer_cast<ResourceDerived>(r);
    }

    PC_LOGERROR("There is no resource with this name " + _name);
    
   return nullptr;
}

template <class ResourceDerived>
std::shared_ptr<ResourceDerived> ResourceManager::Get(const Guid& _guid)
{
    assert(m_ResourcesMap.contains(_guid));


    return m_ResourcesMap.at(_guid);
}

template<class ResourceDerived>
inline bool ResourceManager::TryGetAs(const Guid& _guid, std::shared_ptr<ResourceDerived>* _outPtr)
{
    auto it = m_ResourcesMap.find(_guid);
    if (it == m_ResourcesMap.end())
    {
        PC_LOGERROR("Cant find resource")
        return false;
    }

    if (it->second->GetType().typeId != Reflector::GetTypeKey<ResourceDerived>())
        return false;

    *_outPtr = std::reinterpret_pointer_cast<ResourceDerived>(it->second);

    return true;
}

template <class ResourceDerived>
bool ResourceManager::Exist(const std::string& _name)
{
    return m_NameToGuid.contains(_name);
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
    auto itGuid = m_NameToGuid.find(_name);
    if (itGuid == m_NameToGuid.end())
    {
        PC_LOGERROR("There is no m_NameToGuid map {}", _name);
        return false;
    }
    
    auto it = m_ResourcesMap.find(itGuid->second);

    if (it == m_ResourcesMap.end())
    {
        PC_LOGERROR("There is no m_ResourcesMap map {}", _name);
        return false;
    }
    assert(it->second->name == _name);
    
    PC_LOG("Erase resource name as {}", it->second->name);
    it->second.reset();
    m_ResourcesMap.erase(it);
    
    return true;
}

template <class ResourceDerived>
void ResourceManager::ForEach(const std::function<void(ResourceDerived*)>& _lamba)
{
    const TypeId typeId = Reflector::GetTypeKey<ResourceDerived>();
    
    for (auto it = m_NameToGuid.begin(); it != m_NameToGuid.end(); it++)
    {
        const ResourceDerived* interface = reinterpret_cast<ResourceDerived*>(it->second);
        
        if (typeId != interface->GetType().typeId)
            continue;
        
        _lamba(reinterpret_cast<ResourceDerived*>(it->second));
    }
}




END_PCCORE
