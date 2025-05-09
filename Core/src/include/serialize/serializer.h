﻿#pragma once


#include <filesystem>
#include <Json/json.hpp>
#include <fstream>
using json = nlohmann::json;

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
    PC_CORE_API static void Serializing(const uint8_t* objetPtr, const fs::path& _fileToSerialize, TypeId _typeKey);

    PC_CORE_API static void Derializing(uint8_t* objetPtr, const fs::path& _fileToSerialize, TypeId _typeKey);

};

template <typename T>
void Serializer::Serialize(const T& _object, const fs::path& _fileToSerialize)
{
    static_assert(!std::is_pointer_v<T>, "you shouldn't serialize a raw ptr");

    Serializing(reinterpret_cast<const uint8_t*>(&_object), _fileToSerialize, COMPILE_TIME_TYPE_KEY(T));
}

template <typename T>
void Serializer::DeSerialize(T* _object,const fs::path& _file)
{
    static_assert(!std::is_pointer_v<std::remove_pointer_t<T*>>, "you should deSerialize only to a value");


    Derializing(reinterpret_cast<uint8_t*>(_object), _file, COMPILE_TIME_TYPE_KEY(T));
}

END_PCCORE
