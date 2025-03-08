#pragma once

#include "core_header.hpp"
#include <filesystem>
#include <array>
#include <set>
#include "log.hpp"

#include "guid.hpp"
#include "reflection/reflection_typedef.hpp"
#include "reflection/reflector.hpp"

namespace fs = std::filesystem;



BEGIN_PCCORE

class ResourceManager;


class Resource
{
public:
    std::string name;
    
    std::string extension;

    Guid guid;

    std::set<std::string> resourceDependecies;

    std::string pathToFile;

    // SOULD BE = 0
    PC_CORE_API virtual void Build() {};

    PC_CORE_API Resource& operator=(Resource&& _other) noexcept = default;

    PC_CORE_API Resource(Resource&& _other) noexcept;
    
    PC_CORE_API Resource();

    PC_CORE_API Resource(const Guid& _guid) {}

    PC_CORE_API Resource(const std::string& _name);

    PC_CORE_API Resource(const fs::path& _file);

    PC_CORE_API virtual ~Resource() = default;


};


REFLECT(Resource)
REFLECT_MEMBER(Resource, name)
REFLECT_MEMBER(Resource, guid)
REFLECT_MEMBER(Resource, pathToFile)



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


template <class T>
class ResourceInterface : public Resource
{
public:
    ResourceInterface();

    ResourceInterface(const fs::path& _path);
    
    virtual ~ResourceInterface() override = default;

    const ReflectedType& GetType() const;
private:
    const ReflectedType* m_ReflectedType = nullptr;
};

template <class T>
ResourceInterface<T>::ResourceInterface() : Resource()
{
    static_assert(!std::is_same_v<T, Resource>, "you should not put resource in the template, place your class");

    m_ReflectedType = &Reflector::GetType<T>();
}

template<class T>
inline ResourceInterface<T>::ResourceInterface(const fs::path& _path) : Resource(_path)
{ 
    m_ReflectedType = &Reflector::GetType<T>();
}
template <class T>
inline const ReflectedType& ResourceInterface<T>::GetType() const
{
    return *m_ReflectedType;
}


template<class T>
concept ResourceDerived = std::is_base_of_v<Resource, T>;

template <typename ResourceDerived>
using ResourceRef = std::weak_ptr<ResourceDerived>;


END_PCCORE
