#pragma once
#include <fstream>

#include "CoreHeader.hpp"
#include "Reflection/Reflector.hpp"

// unordoredMap Function
using UnordoredByteMap = std::unordered_map<uint8_t, uint8_t>;
using UnordoredMapIterator = typename UnordoredByteMap::iterator;
using IncrementMapIterator = UnordoredMapIterator & (UnordoredMapIterator::*)();
using ReseverMapFunction = void (UnordoredByteMap::*)(size_t);
using InsertMapFunction = typename UnordoredByteMap::mapped_type& (UnordoredByteMap::*)(const typename UnordoredByteMap::key_type&);

using UnordoredMapConstIterator = typename UnordoredByteMap::const_iterator;
using UnorderedMapUnrefConstIteratorFunc = const std::pair<const typename UnordoredByteMap::key_type, typename UnordoredByteMap::mapped_type>* (UnordoredMapConstIterator::*)() const;
//


BEGIN_PCCORE


class PC_CORE_API Serializer
{
public:

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

    template<typename... T>
    bool Serialize(const T&... _object, const std::string& _fileToSerialize)
    {
        PERF_REGION_SCOPED;

        static_assert((... && !std::is_pointer_v<std::remove_reference_t<T>>),
            "you shouldn't serialize a raw ptr");

        if (!OpenFileForWrite(_fileToSerialize))
            return false;

        (Serializing(reinterpret_cast<const uint8_t*>(&std::as_const(_object)),
            COMPILE_TIME_TYPE_KEY(std::remove_cv_t<std::remove_reference_t<T>>)), ...);

        CloseForWrite(_fileToSerialize);

        return true;
    }

    template<typename... T>
    bool DeSerialize(T*... _object, const std::string& _file)
    {
        PERF_REGION_SCOPED;

        static_assert((... && !std::is_pointer_v<T>),
            "You should deSerialize only to a value type, not a pointer-to-pointer");

        if (!OpenFileForRead(_file))
            return false;

        (DeSerializing(reinterpret_cast<uint8_t*>(_object), COMPILE_TIME_TYPE_KEY(T)), ...);

        CloseForRead(_file);

        return true;
    }

    template<typename... SerializeEntry>
    bool SerializeEntries(const std::string& _file, const SerializeEntry&... _entries)
    {
        PERF_REGION_SCOPED;

        if (!OpenFileForWrite(_file))
            return false;

        (Serializing(reinterpret_cast<const uint8_t* const>(_entries.object), _entries.id), ...);
        CloseForWrite(_file);

        return true;
    }

    template<typename... DeserializeEntry>
    bool DeSerializeEntries(const std::string& _file, DeserializeEntry&... _entries)
    {
        PERF_REGION_SCOPED;
     
        if (!OpenFileForRead(_file))
            return false;

        (DeSerializing(reinterpret_cast<uint8_t*>(_entries.object), _entries.id), ...);
        CloseForRead(_file);

        return true;
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

    virtual void DeSerializeTrivial(PC_CORE::TypeId id, uint8_t* objetPtr) = 0;

    virtual bool OpenFileForRead(const std::string& _fileToSerialize) = 0;

    virtual void CloseForRead(const std::string& _fileToSerialize) = 0;

    virtual bool OpenFileForWrite(const std::string& _fileToSerialize) = 0;

    virtual void CloseForWrite(const std::string& _fileToSerialize) = 0;


    std::ifstream m_Instream;

    std::ofstream m_OutStream;
};


END_PCCORE
