#pragma once

#include "Serialize/Serializer.h"

#include <String>
#include <nlohmann/json.hpp>

using json = nlohmann::json;


BEGIN_PCCORE
    class PC_CORE_API JsonSerializer final : public Serializer
    {
    public:
        bool IsOpen() const override;

        void OpenFile(const std::string& _path, SerializeOperation _operation) override;

        void CloseFile() override;

        void SerializeCompactBuffer(const char* _key, const CompactBuffer& _compactBuffer) override;

        void DeSerializeCompactBuffer(const char* _key, CompactBuffer* _compactBuffer) override;


        DEFAULT_COPY_MOVE_OPERATIONS(JsonSerializer)

        explicit JsonSerializer() = default;

        ~JsonSerializer() override
        {
            assert(m_SerializeOperation == SerializeOperation::None && "Did you forgot to call CloseFile");
        };

    private:
        static constexpr auto CONTAINER_SIZE = "size";
        static constexpr auto RESOURCE_TYPE = "resourceType";
        static constexpr auto KEY = "key";
        static constexpr auto VALUE = "value";
        static constexpr auto DATA = "data";
        static constexpr auto GUID_KEY = "Guid";

        static constexpr auto boolAlpha0s = "false";
        static constexpr auto boolAlpha1s = "true";
        static constexpr bool boolAlpha0b = false;
        static constexpr bool boolAlpha1b = true;

        static constexpr auto SPARSE_SET_DENSE = "dense";
        static constexpr auto SPARSE_SET_SPARSE = "sparse";

        static constexpr auto RESOURCE_OBJECT = "Object";
        static constexpr auto OBJECT_TYPE = "ObjectType";


        json m_MainJson;

        std::vector<json*> m_JsonStack;

        json& GetLastJson()
        {
            return *m_JsonStack.back();
        }

        void SerializeMember(const Members& member, const uint8_t* objetPtr) override;

        void SerializeType(const uint8_t* objetPtr, TypeId _typeKey) override;

        void DeSerializeMember(const Members& member, uint8_t* objetPtr) override;

        void SerializeTrivial(TypeId id, const uint8_t* objetPtr) override;

        void DeSerializeTrivial(TypeId id, uint8_t* objetPtr) override;

        // Inherited via Serializer
        void Serializing(const uint8_t* objetPtr, TypeId _typeKey) override;

        void DeSerializing(uint8_t* objetPtr, TypeId _typeKey) override;

        void DeserializeType(uint8_t* objetPtr, TypeId _typeKey) override;
    };

END_PCCORE
