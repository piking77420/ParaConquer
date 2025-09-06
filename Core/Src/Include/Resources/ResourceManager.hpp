#pragma once

#include <functional>
#include <map>
#include <memory>


#include "CoreHeader.hpp"
#include "Singleton.hpp"
#include "Guid.hpp"
#include "Log.hpp"
#include "Resource.hpp"

BEGIN_PCCORE

template <typename ResourceDerived>
using ResourceHandle = std::shared_ptr<ResourceDerived>;

class PC_CORE_API ResourceManager : public Singleton<ResourceManager>
{
public:
    static void InitPath();

    static void Destroy();

    template<class ResourceDerived, typename... Arg>
    static std::shared_ptr<ResourceDerived>  Create(Arg... args);

    template<class ResourceDerived, typename... Arg>
    static std::shared_ptr<ResourceDerived>  CreateFromPath(Arg... args);
    
    template<class ResourceDerived>
    static std::shared_ptr<ResourceDerived> Get(const std::string& _name);

    template<class ResourceDerived>
    static std::shared_ptr<ResourceDerived> Get(const Guid& _guid);

    template<class ResourceDerived>
    static bool TryGetAs(const Guid& _guid, std::shared_ptr<ResourceDerived>* _outPtr);

    static const std::string& GetName(const Guid& _guid); 
    
    template<class ResourceDerived>
    static bool Exist(const std::string& _name);

    static bool Exist(const Guid& _guid);

    template<class ResourceDerived>
    static std::shared_ptr<ResourceDerived> Get();
    
    template<class ResourceDerived>
    static bool Delete(const std::string& _name);

    template <class ResourceDerived>
    static void ForEach(const std::function<void(ResourceDerived*)>& _lamba);
    
    static void ForEach(TypeId typeID, const std::function<void(std::shared_ptr<Resource>)>& _lamba);


private:
    std::unordered_map<Guid, std::shared_ptr<Resource>> m_ResourcesMap;

    std::unordered_map<std::string, Guid> m_NameToGuid;

    std::filesystem::path m_BasePath;

    static void SerializeResource();

    static void DeserializeResource();

    REFLECT(ResourceManager);
    REFLECT_MEMBER(ResourceManager, m_ResourcesMap);
    REFLECT_MEMBER(ResourceManager, m_NameToGuid);

    REFLECT(std::shared_ptr<Resource>)
};



template<class ResourceDerived, typename... Arg>
std::shared_ptr<ResourceDerived> ResourceManager::Create(Arg... args)
{

    std::shared_ptr<ResourceDerived> newR = std::make_shared<ResourceDerived>(std::forward<Arg>(args)...);
    newR->m_IsLoaded = true;
    
    auto& resourcesMap = Instance().m_ResourcesMap;
    resourcesMap.insert({newR->GetGuid(), newR});
    auto& nameToGuid = Instance().m_NameToGuid;

    assert(!nameToGuid.contains(newR->name) && "There is a resource with the same name already");

    nameToGuid.insert({ newR->name, newR->GetGuid() });

    return std::reinterpret_pointer_cast<ResourceDerived>(newR);
}



template <class ResourceDerived>
std::shared_ptr<ResourceDerived> ResourceManager::Get(const std::string& _name)
{
    auto it = Instance().m_NameToGuid.find(_name);
    if (it != Instance().m_NameToGuid.end())
    {
        auto r = Instance().m_ResourcesMap.at(it->second);
        assert(r->GetGuid() == it->second);
        assert(r->name ==  _name);
        assert(std::dynamic_pointer_cast<ResourceDerived>(r) != nullptr);
        
        return std::reinterpret_pointer_cast<ResourceDerived>(r);
    }

    PC_LOGERROR("There is no resource with this name " + _name);
    
   return nullptr;
}

template <class ResourceDerived>
std::shared_ptr<ResourceDerived> ResourceManager::Get(const Guid& _guid)
{
    assert(Instance().m_ResourcesMap.contains(_guid));


    return Instance().m_ResourcesMap.at(_guid);
}

template<class ResourceDerived>
inline bool ResourceManager::TryGetAs(const Guid& _guid, std::shared_ptr<ResourceDerived>* _outPtr)
{
    auto it = Instance().m_ResourcesMap.find(_guid);
    if (it == Instance().m_ResourcesMap.end())
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
    return Instance().m_NameToGuid.contains(_name);
}

template <class ResourceDerived>
std::shared_ptr<ResourceDerived> ResourceManager::Get()
{
    for (auto it = Instance().m_ResourcesMap.begin(); it != Instance().m_ResourcesMap.end(); it++)
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
    auto itGuid = Instance().m_NameToGuid.find(_name);
    if (itGuid == Instance().m_NameToGuid.end())
    {
        PC_LOGERROR("There is no m_NameToGuid map {}", _name);
        return false;
    }
    
    auto it = Instance().m_ResourcesMap.find(itGuid->second);

    if (it == Instance().m_ResourcesMap.end())
    {
        PC_LOGERROR("There is no m_ResourcesMap map {}", _name);
        return false;
    }
    assert(it->second->name == _name);
    
    PC_LOG("Erase resource name as {}", it->second->name);
    it->second.reset();
    Instance().m_ResourcesMap.erase(it);
    
    return true;
}

template <class ResourceDerived>
void ResourceManager::ForEach(const std::function<void(ResourceDerived*)>& _lamba)
{
    const TypeId typeId = Reflector::GetTypeKey<ResourceDerived>();
    
    for (auto it = Instance().m_NameToGuid.begin(); it != Instance().m_NameToGuid.end(); it++)
    {
        const ResourceDerived* interface = reinterpret_cast<ResourceDerived*>(it->second);
        
        if (typeId != interface->GetType().typeId)
            continue;
        
        _lamba(reinterpret_cast<ResourceDerived*>(it->second));
    }
}




END_PCCORE
