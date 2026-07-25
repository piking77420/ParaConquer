#pragma once
#include <fstream>

#include "CoreHeader.hpp"
#include "Reflection/Reflector.hpp"
#include "CompactBuffer.hpp"

// unordoredMap Function
using UnordoredByteMap = std::unordered_map<uint8_t, uint8_t>;
using UnordoredMapIterator = UnordoredByteMap::iterator;
using IncrementMapIterator = UnordoredMapIterator & (UnordoredMapIterator::*)();
using ReseverMapFunction = void (UnordoredByteMap::*)(size_t);
using InsertMapFunction = UnordoredByteMap::mapped_type& (UnordoredByteMap::*)(const UnordoredByteMap::key_type&);

using UnordoredMapConstIterator = UnordoredByteMap::const_iterator;
using UnorderedMapUnrefConstIteratorFunc = const std::pair<
    const UnordoredByteMap::key_type, UnordoredByteMap::mapped_type>* (UnordoredMapConstIterator::*)() const;


BEGIN_PCCORE
    class PC_CORE_API Serializer
    {
    public:
        enum class SerializeOperation : uint8_t
        {
            Serialize = 1,
            DeSerialize = 2
        };

        struct SerializeEntry
        {
            TypeId id = NullTypeId;
            const void* const object = nullptr;

            SerializeEntry(TypeId _id, void* _ptr) : id(_id), object(_ptr)
            {
            }

            SerializeEntry() = default;

            ~SerializeEntry() = default;
        };

        struct DeserializeEntry
        {
            TypeId id = NullTypeId;
            void* object = nullptr;

            DeserializeEntry(TypeId _id, void* _ptr) : id(_id), object(_ptr)
            {
            }

            DeserializeEntry() = default;

            ~DeserializeEntry() = default;
        };


        virtual bool OpenFile(const std::string& _path, SerializeOperation _operation)
        {
            assert(m_CurrentFilePath == "" && "Did you forget to call CloseFile");

            m_SerializeOperation = _operation;
            m_CurrentFilePath = _path;

            return true;
        }

        virtual bool CloseFile()
        {
            assert(m_CurrentFilePath != "" && "Did you forget to call OpenFile");

            m_CurrentFilePath = "";

            return true;
        }

        template <typename... T>
        void Serialize(const T&... _object)
        {
            PERF_REGION_SCOPED;

            static_assert((... && !std::is_pointer_v<std::remove_reference_t<T>>),
                          "you shouldn't serialize a raw ptr");

            (Serializing(reinterpret_cast<const uint8_t*>(&std::as_const(_object)),
                         COMPILE_TIME_TYPE_KEY(std::remove_cv_t<std::remove_reference_t<T>>)), ...);
        }

        template <typename... T>
        void DeSerialize(T*... _object)
        {
            PERF_REGION_SCOPED;

            static_assert((... && !std::is_pointer_v<T>),
                          "You should deserialize only to value types, not pointer-to-pointer");

            (DeSerializing(reinterpret_cast<uint8_t*>(_object), COMPILE_TIME_TYPE_KEY(T)), ...);
        }

        template <typename... SerializeEntry>
        void SerializeEntries(const SerializeEntry&... _entries)
        {
            PERF_REGION_SCOPED;
            (Serializing(reinterpret_cast<const uint8_t* const>(_entries.object), _entries.id), ...);
        }

        template <typename... DeserializeEntry>
        void DeSerializeEntries(DeserializeEntry&... _entries)
        {
            PERF_REGION_SCOPED;
            (DeSerializing(reinterpret_cast<uint8_t*>(_entries.object), _entries.id), ...);
        }

        template <typename... T>
        void SerializeStream(const T&... _object)
        {
            PERF_REGION_SCOPED;

            static_assert((... && !std::is_pointer_v<std::remove_reference_t<T>>),
                          "you shouldn't serialize a raw ptr");

            (SerializeType(reinterpret_cast<const uint8_t*>(std::addressof(_object)),
                           Reflector::GetTypeKey<T>()), ...);
        }

        template <typename... T>
        void DeSerializeStream(T&... _object)
        {
            PERF_REGION_SCOPED;

            static_assert((... && !std::is_pointer_v<std::remove_reference_t<T>>),
                          "You shouldn't deserialize a raw ptr");

            (DeserializeType(reinterpret_cast<uint8_t*>(std::addressof(_object)),
                             Reflector::GetTypeKey<T>()), ...);
        }

        virtual void SerializeCompactBuffer(const char* _key, const CompactBuffer& _compactBuffer) = 0;

        virtual void DeSerializeCompactBuffer(const char* _key, CompactBuffer* _compactBuffer) = 0;

        virtual bool IsOpen() const = 0;

        SerializeOperation GetCurrentOperation() const
        {
            return m_SerializeOperation;
        }

        const std::string& GetCurrentFilePath() const
        {
            return m_CurrentFilePath;
        }

        Serializer() = default;

        virtual ~Serializer() = default;

    protected:
        virtual void Serializing(const uint8_t* objetPtr, TypeId _typeKey) = 0;

        virtual void DeSerializing(uint8_t* objetPtr, TypeId _typeKey) = 0;

        virtual void SerializeMember(const Members& member, const uint8_t* objetPtr) = 0;

        virtual void SerializeType(const uint8_t* objetPtr, TypeId _typeKey) = 0;

        virtual void DeSerializeMember(const Members& member, uint8_t* objetPtr) = 0;

        virtual void DeserializeType(uint8_t* objetPtr, TypeId _typeKey) = 0;

        virtual void SerializeTrivial(TypeId id, const uint8_t* objetPtr) = 0;

        virtual void DeSerializeTrivial(TypeId id, uint8_t* objetPtr) = 0;

        std::ifstream m_Instream;

        std::ofstream m_OutStream;

        SerializeOperation m_SerializeOperation = SerializeOperation::Serialize;

        std::string m_CurrentFilePath;
    };


END_PCCORE
