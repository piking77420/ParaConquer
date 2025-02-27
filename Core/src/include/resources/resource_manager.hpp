#pragma once

#include <functional>
#include <map>

#include "core_header.hpp"
#include "guid.hpp"
#include "log.hpp"
#include "resource.hpp"

BEGIN_PCCORE
class ResourceManager
{
public:
    ResourceManager() = delete;

    ~ResourceManager() = delete;

    PC_CORE_API static void InitPath();

    PC_CORE_API static void Destroy();

    template<class T>
    static std::shared_ptr<T>  Create(const fs::path& path);

    template<class T, typename... Arg>
    static std::shared_ptr<T>  Create(Arg... args);

    template<class T>
    static void Add(const std::string& _name, std::shared_ptr<Resource> _resource);

    template<class T>
    static void Add(std::shared_ptr<Resource> _resource);
    
    template<class T>
    static std::shared_ptr<T> Get(const std::string& _name);

    template<class T>
    static std::shared_ptr<T> Get();

    static std::shared_ptr<Resource> GetByGuid(const Guid& _guid);

    template<class T>
    static bool Delete(const std::string& _name);

    template <class T>
    static void ForEach(const std::function<void(T*)>& _lamba);
    
    PC_CORE_API static void ForEach(TypeId typeID, const std::function<void(std::shared_ptr<Resource>)>& _lamba);

 
private:
    struct PathName
    {
        fs::path path = {};
        std::string name = {};
    };
    
    PC_CORE_API static inline std::map<fs::path, std::shared_ptr<Resource>> m_ResourcesMap;
};

template <class T>
std::shared_ptr<T> ResourceManager::Create(const fs::path& path)
{
    static_assert(std::is_base_of_v<Resource,T>,"T is not a resource");
    
    std::shared_ptr<Resource> newR = std::make_shared<T>(path);
   
    m_ResourcesMap.emplace(path, newR);

    return newR;
}

template<class T, typename... Arg>
std::shared_ptr<T> ResourceManager::Create(Arg... args)
{
    static_assert(std::is_base_of_v<Resource, T>, "T is not a resource");

    std::shared_ptr<T> newR = std::make_shared<T>(std::forward<Arg>(args)...);

    m_ResourcesMap.emplace(newR->name, newR);

    return newR;
}

template <class T>
void ResourceManager::Add(const std::string& _name, std::shared_ptr<Resource> _resource)
{
    _resource->name = _name;
    m_ResourcesMap.emplace(_name,_resource);
}

template <class T>
void ResourceManager::Add(std::shared_ptr<Resource> _resource)
{
    m_ResourcesMap.emplace(_resource->name, _resource);
}

template <class T>
std::shared_ptr<T> ResourceManager::Get(const std::string& _name)
{
    for (auto it = m_ResourcesMap.begin(); it != m_ResourcesMap.end(); it++)
    {
        if (it->second->name == _name)
            return std::reinterpret_pointer_cast<T>(it->second);
    }
    PC_LOGERROR("There is no resource with this name " + _name);
    
   return nullptr;
}

template <class T>
std::shared_ptr<T> ResourceManager::Get()
{
    for (auto it = m_ResourcesMap.begin(); it != m_ResourcesMap.end(); it++)
    {
        if (dynamic_cast<T*>(it->second))
            return it->second;
    }
    PC_LOGERROR("There is no resource as this type");

    return nullptr;
}



template <class T>
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

template <class T>
void ResourceManager::ForEach(const std::function<void(T*)>& _lamba)
{
    const TypeId typeId = Reflector::GetTypeKey<T>();
    
    for (auto it = m_ResourcesMap.begin(); it != m_ResourcesMap.end(); it++)
    {
        const ResourceInterface<T>* interface = reinterpret_cast<T*>(it->second);
        
        if (typeId != interface->GetType().typeId)
            continue;
        
        _lamba(reinterpret_cast<T*>(it->second));
    }
}

END_PCCORE
