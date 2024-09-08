#pragma once

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

    PC_CORE_API static void Init();

    PC_CORE_API static void Destroy();

    template<class T>
    static T* CreateAndLoad(const fs::path& path);

    template<class T>
    static void Add(const std::string& _name,T* _resource);
    
    template<class T>
    static T* Get(const std::string& _name);

    template<class T>
    static bool Delete(const std::string& _name);

private:
    struct PathName
    {
        fs::path path = {};
        std::string name = {};
    };
    
    PC_CORE_API static inline std::map<fs::path, IResource*> m_ResourcesMap;
};

template <class T>
T* ResourceManager::CreateAndLoad(const fs::path& path)
{
    static_assert(std::is_base_of_v<IResource,T>,"T is not a resource");
    
    T* newR = new T;
    newR->Load(path);
    
    
    m_ResourcesMap.emplace(path,newR);

    return newR;
}

template <class T>
void ResourceManager::Add(const std::string& _name,T* _resource)
{
    _resource->name = _name;
    m_ResourcesMap.emplace(_name,_resource);
}

template <class T>
T* ResourceManager::Get(const std::string& _name)
{
    for (auto it = m_ResourcesMap.begin(); it != m_ResourcesMap.end(); it++)
    {
        if (it->second->name == _name)
            return reinterpret_cast<T*>(it->second);
    }
    PC_LOGERROR("There is no resource with this name " + _name);
    
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

END_PCCORE
