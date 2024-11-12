#pragma once

#include <filesystem>

#include "core_header.hpp"
#include "reflection/reflector.hpp"

BEGIN_PCCORE
    namespace fs = std::filesystem;

class Serializer
{
public:
    template<typename T>
    static void Serialize(const T& _object, const fs::path& _fileToSerialize);
    
    template<typename T>
    static void DeSerialize(T* _object, const fs::path& _file);
    
private:
    PC_CORE_API static void Serializing(const uint8_t* objetPtr, const fs::path& _fileToSerialize, uint32_t _typeKey);

    PC_CORE_API static void Derializing(uint8_t* objetPtr, const fs::path& _fileToSerialize, uint32_t _typeKey);
};

template <typename T>
void Serializer::Serialize(const T& _object, const fs::path& _fileToSerialize)
{
    Serializing(reinterpret_cast<const uint8_t*>(&_object), _fileToSerialize, Reflector::GetKey<T>());
}

template <typename T>
void Serializer::DeSerialize(T* _object,const fs::path& _file)
{
    Derializing(reinterpret_cast<uint8_t*>(_object), _file, Reflector::GetKey<T>());
}

END_PCCORE
