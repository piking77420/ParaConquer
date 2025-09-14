#pragma once

#include <functional>
#include <map>
#include <memory>
#include "ObjectPtr.hpp"

#include "CoreHeader.hpp"
#include "Singleton.hpp"
#include "Guid.hpp"
#include "Log.hpp"
#include "Resource.hpp"

BEGIN_PCCORE

class PC_CORE_API ResourceManager : public Singleton<ResourceManager>
{
public:
    static void InitPath();

    static void Destroy();

    template<class ResourceDerived>
    [[nodiscard]] static bool Add(const ObjectPtr<ResourceDerived>& _object);

    template<class ResourceDerived, typename... Arg>
    static ObjectPtr<ResourceDerived>  Create(Arg... args);

    template<class ResourceDerived>
    static ObjectPtr<ResourceDerived> Get(const std::string& _name);

    template<class ResourceDerived>
    static ObjectPtr<ResourceDerived> Get(const Guid& _guid);

    template<class ResourceDerived>
    static bool TryGetAs(const Guid& _guid, ObjectPtr<ResourceDerived>* _outPtr);

    static const std::string& GetName(const Guid& _guid);

    static bool Exist(const std::string& _name);

    static bool Exist(const Guid& _guid);

    template<class ResourceDerived>
    static ObjectPtr<ResourceDerived> Get();

    template<class ResourceDerived>
    static bool Delete(const std::string& _name);

    template <class ResourceDerived>
    static void ForEach(const std::function<void(ResourceDerived*)>& _lamba);

    static void ForEach(TypeId typeID, const std::function<void(std::shared_ptr<Resource>)>& _lamba);


private:
    std::unordered_map<Guid, ObjectPtr<Resource>> m_ResourcesMap;

    std::unordered_map<std::string, Guid> m_NameToGuid;

    REFLECT(ResourceManager);
    REFLECT(PC_CORE::ObjectPtr<PC_CORE::Resource>);
};



template<class ResourceDerived, typename... Arg>
ObjectPtr<ResourceDerived> ResourceManager::Create(Arg... args)
{
    ObjectPtr<ResourceDerived> newR = std::make_shared<ResourceDerived>(std::forward<Arg>(args)...);
    newR->m_IsLoaded = true;

    if (!Add(newR))
        return nullptr;

    return std::reinterpret_pointer_cast<ResourceDerived>(newR);
}

template<class ResourceDerived>
bool ResourceManager::Add(const ObjectPtr<ResourceDerived>& _object)
{
    auto& resourcesMap = Instance().m_ResourcesMap;
    auto& nameToGuid = Instance().m_NameToGuid;

    const bool guidExist = resourcesMap.contains(_object->GetGuid());
    if (guidExist)
    {
        PC_LOGERROR("There is already a resource with this guid {}", static_cast<std::string>(_object->GetGuid()));
        return false;
    }
    const bool nameExist = nameToGuid.contains(_object->name);
    if (nameExist)
    {
        PC_LOGERROR("There is already a resource with this name {}", _object->name);
        return false;
    }
    resourcesMap.insert({ _object->GetGuid(), _object });
    nameToGuid.insert({ _object->name, _object->GetGuid() });

    return true;
}


template <class ResourceDerived>
ObjectPtr<ResourceDerived> ResourceManager::Get(const std::string& _name)
{
    auto it = Instance().m_NameToGuid.find(_name);
    if (it != Instance().m_NameToGuid.end())
    {
        auto r = Instance().m_ResourcesMap.at(it->second);
        assert(r->GetGuid() == it->second);
        assert(r->name == _name);
        assert(std::dynamic_pointer_cast<ResourceDerived>(r) != nullptr);

        return std::reinterpret_pointer_cast<ResourceDerived>(r);
    }

    PC_LOGERROR("There is no resource with this name " + _name);

    return nullptr;
}

template <class ResourceDerived>
ObjectPtr<ResourceDerived> ResourceManager::Get(const Guid& _guid)
{
    assert(Instance().m_ResourcesMap.contains(_guid));


    return Instance().m_ResourcesMap.at(_guid);
}

template<class ResourceDerived>
inline bool ResourceManager::TryGetAs(const Guid& _guid, ObjectPtr<ResourceDerived>* _outPtr)
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
ObjectPtr<ResourceDerived> ResourceManager::Get()
{
    for (auto it = Instance().m_ResourcesMap.begin(); it != Instance().m_ResourcesMap.end(); it++)
    {
        return std::dynamic_pointer_cast<ResourceDerived>(it->second);
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
