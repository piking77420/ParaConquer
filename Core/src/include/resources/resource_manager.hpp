﻿#pragma once

#include <map>

#include "core_header.hpp"
#include "guid.hpp"
#include "resource.hpp"

BEGIN_PCCORE
class ResourceManager
{
public:
    ResourceManager() = delete;

    ~ResourceManager() = delete;

    static void Init();

    static void Destroy();

    template<class T>
    static T* CreateAndLoad(const fs::path& path);
    
    template<class T>
    static T* Get(const std::string& _name);

private:
    struct PathName
    {
        fs::path path = {};
        std::string name = {};
    };
    
    static inline std::map<fs::path, IResource*> m_ResourcesMap;
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
T* ResourceManager::Get(const std::string& _name)
{
    for (auto it = m_ResourcesMap.begin(); it != m_ResourcesMap.end(); it++)
    {
        if (it->second->name == _name)
            return reinterpret_cast<T*>(it->second);
    }
   return nullptr;
}

END_PCCORE
