#pragma once

#include "Serialize/Serializer.h"

#include <String>
#include <Json/Json.hpp>
using json = nlohmann::json;


BEGIN_PCCORE

class PC_CORE_API JsonSerializer final : public PC_CORE::Serializer
{
public:
    bool IsOpen() const override;
public:

    void OpenFile(const std::string& _path, SerializeOperation _operation) override;

    void CloseFile() override;

    DEFAULT_COPY_MOVE_OPERATIONS(JsonSerializer)

	explicit JsonSerializer() = default;
    
	~JsonSerializer() override
	{
        assert(m_SerializeOperation == SerializeOperation::None && "Did you forgot to call CloseFile");
	};

private:
    static constexpr const char* CONTAINER_SIZE = "size";
    static constexpr const char* RESOURCE_TYPE = "resourceType";
    static constexpr const char* KEY = "key";
    static constexpr const char* VALUE = "value";
    static constexpr const char* DATA = "data";
    static constexpr const char* GUID_KEY = "Guid";

    static constexpr const char* boolAlpha0s = "false";
    static constexpr const char* boolAlpha1s = "true";
    static constexpr bool boolAlpha0b = false;
    static constexpr bool boolAlpha1b = true;

    static constexpr const char* SPARSE_SET_DENSE = "dense";
    static constexpr const char* SPARSE_SET_SPARSE = "sparse";

    static constexpr const char* RESOURCE_OBJECT = "Object";
    static constexpr const char* OBJECT_TYPE = "ObjectType";


    json m_MainJson;

    std::vector<json*> m_JsonStack;

    inline json& GetLastJson()
    {
        return *m_JsonStack.back();
    }

    void SerializeMember(const PC_CORE::Members& member, const uint8_t* objetPtr) override;

    void SerializeType(const uint8_t* objetPtr, PC_CORE::TypeId _typeKey) override;

    void DeSerializeMember(const PC_CORE::Members& member, uint8_t* objetPtr) override;

    void SerializeTrivial(PC_CORE::TypeId id, const uint8_t* objetPtr) override;

    void DeSerializeTrivial(PC_CORE::TypeId id, uint8_t* objetPtr) override;

    // Inherited via Serializer
    void Serializing(const uint8_t* objetPtr, TypeId _typeKey) override;

    void DeSerializing(uint8_t* objetPtr, TypeId _typeKey) override;

    void DeserializeType(uint8_t* objetPtr, TypeId _typeKey) override;

   
};

END_PCCORE