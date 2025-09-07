#pragma once


//#include <filesystem>
#include <String>
#include <Json/Json.hpp>
using json = nlohmann::json;

#include "CoreHeader.hpp"
#include "Reflection/Reflector.hpp"




BEGIN_PCCORE

class PC_CORE_API Serializer
{
public:
	static constexpr const char* CONTAINER_SIZE = "size";
	static constexpr const char* RESOURCE_TYPE = "resourceType";
	static constexpr const char* KEY = "key";
	static constexpr const char* VALUE = "value";
	static constexpr const char* DATA = "data";
    static constexpr const char* GUID_KEY = "Guid";


	static constexpr const char* SPARSE_SET_DENSE = "dense";
	static constexpr const char* SPARSE_SET_SPARSE = "sparse";

    
    template<typename T>
    static void Serialize(const T& _object, const std::string& _fileToSerialize);
    
    template<typename T>
    static void DeSerialize(T* _object, const std::string& _file);
    
    static void Serialize(TypeId _id, const void* const _object, std::string& _fileToSerialize)
    {
        Serializing(static_cast<const uint8_t*>(_object), _id, _fileToSerialize);
    }
    
    static void DeSerialize(TypeId _id, void* _object, std::string& _file)
    {
        Derializing(static_cast<uint8_t*>(_object), _id, _file);
    }
    static void SerializeMember(json& _jsonFile, const Members& member, const uint8_t* objetPtr);

    static void SerializeType(json& _jsonFile, const uint8_t* objetPtr, TypeId _typeKey);

    static void DeSerializeMember(const json& _jsonFile, const Members& member, uint8_t* objetPtr);

    static void DeserializeType(const json& _jsonFile, uint8_t* objetPtr, TypeId _typeKey);
    
private:    
    static void Serializing(const uint8_t* objetPtr, TypeId _typeKey, const std::string& _fileToSerialize);

    static void Derializing(uint8_t* objetPtr, TypeId _typeKey, const std::string& _fileToSerialize);

};

template <typename T>
void Serializer::Serialize(const T& _object, const std::string& _fileToSerialize)
{
    PERF_REGION_SCOPED;
    static_assert(!std::is_pointer_v<T>, "you shouldn't serialize a raw ptr");
    Serializing(reinterpret_cast<const uint8_t*>(&_object), COMPILE_TIME_TYPE_KEY(T), _fileToSerialize);
}

template <typename T>
void Serializer::DeSerialize(T* _object,const std::string& _file)
{
    PERF_REGION_SCOPED;
    static_assert(!std::is_pointer_v<std::remove_pointer_t<T*>>, "you should deSerialize only to a value");

    Derializing(reinterpret_cast<uint8_t*>(_object), COMPILE_TIME_TYPE_KEY(T), _file);
}

END_PCCORE
