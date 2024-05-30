#pragma once

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

END_PCCORE
