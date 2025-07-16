#pragma once

#include "core_header.hpp"
#include <array>
#include <set>
#include "log.hpp"

#include "guid.hpp"
#include "reflection/reflection_typedef.hpp"
#include "reflection/reflector.hpp"
#include "serialize/iseriazable.h"




BEGIN_PCCORE

class ResourceManager;


class Resource : public ISeriazable
{
public:

    std::string name;
    
    std::string extension;

    PC_CORE_API virtual void LoadFromFile(const std::string& _path);
    
    PC_CORE_API void QueryType() override = 0;
    // SOULD BE = 0
    PC_CORE_API virtual void Build() {};

    // Reload base on modificated parent ?
    PC_CORE_API virtual void OnParentReload(const Guid& _parentGuid)
    {
        PC_LOG("OnParentReload {}", name)
    };

    PC_CORE_API virtual void Reload()
    {
        PC_LOG("Reload {}", name)
    }

    static PC_CORE_API void LinkDependencies(Resource* _resourceParent,  Resource* _resourceChild);

    // Reload 
    PC_CORE_API void BroadCastReload();
    
    PC_CORE_API const Guid& GetGuid() const
    {
        return m_Guid;
    }

    PC_CORE_API const std::atomic<bool>& IsLoaded() const ;

    PC_CORE_API Resource& operator=(const Resource& _other) noexcept;

    PC_CORE_API Resource(const Resource& _other) noexcept;

    PC_CORE_API Resource& operator=(Resource&& _other) noexcept;

    PC_CORE_API Resource(Resource&& _other) noexcept;
    
    PC_CORE_API Resource() : m_Guid(Guid::New())
    {

    }

    PC_CORE_API Resource(const Guid& _guid) : m_Guid(_guid)
    {
        
    }
    PC_CORE_API Resource(const std::string& _name);
    
    PC_CORE_API Resource(std::string&& _name);

    PC_CORE_API virtual ~Resource() = default;
    
protected:

    Guid m_Guid;

    const std::vector<Guid>& GetParentResource() const
    {
        return m_ParentsResource;
    }

    const std::vector<Guid>& GetChildResource() const
    {
        return m_ChildsResource;
    }

private:
    friend ResourceManager; // Only the ResourceManager is friend, he in charge of loading resource after all 

    std::vector<Guid> m_ParentsResource;

    std::vector<Guid> m_ChildsResource;

    std::atomic<bool> m_IsLoaded;
    
    REFLECT(Resource)
    REFLECT_MEMBER(Resource, name)
    REFLECT_MEMBER(Resource, m_Guid)
    REFLECT_MEMBER(Resource, m_ParentsResource)
    REFLECT_MEMBER(Resource, m_ChildsResource)
};





template<class T>
concept ResourceDerived = std::is_base_of_v<Resource, T>;

template <typename ResourceDerived>
using ResourceRef = std::weak_ptr<ResourceDerived>;


template <size_t Size>
static bool IsFormatValid(const std::array<std::string, Size>& _format, const std::string& _fileFormat, uint32_t* _formatIndex);

template <typename T, size_t Size>
static bool GetFormatFromValue(const std::array<std::string, Size>& _format, T value, const char** _formatOut);


template <size_t Size>
bool IsFormatValid(const std::array<std::string, Size>& _format, const std::string& _fileFormat, uint32_t* _formatIndex)
{

    for (size_t i = 0; i < _format.size(); i++)
    {
        if (_format[i] == _fileFormat)
        {
            *_formatIndex = static_cast<uint32_t>(i);
            return true;
        }
    }

    _formatIndex = nullptr;
    return false;
}


template <typename T,size_t Size>
bool GetFormatFromValue(const std::array<std::string, Size>& _format, T value, const char** _formatOut)
{

    for (size_t i = 0; i < _format.size(); i++)
    {
        T v = static_cast<T>(i);

        if (value == v)
        {
            *_formatOut = _format[i].c_str();
            return true;
        }
    }
    return false;
}



END_PCCORE
