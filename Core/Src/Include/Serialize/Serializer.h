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
        TypeId id;
        const void* const object;
    };

    struct DeserializeEntry
    {
        TypeId id;
        void* const object;
    };

    template<typename... T>
    void Serialize(const T&... _object, const std::string& _fileToSerialize);

    template<typename... T>
    void DeSerialize(T*... _object, const std::string& _file);

    template<typename... SerializeEntry>
    void SerializeEntries(const SerializeEntry&... _entries, const std::string& _file)
    {
        PERF_REGION_SCOPED;

        OpenFileForRead(_file);
        (Serializing(_entries.id, _entries.object), ...);
        CloseForRead(_file);
    }

    template<typename... DeserializeEntry>
    void DeSerializeEntries(const DeserializeEntry&... _entries, std::string& _file)
    {
        PERF_REGION_SCOPED;
     
        OpenFileForRead(_file);
        (DeSerializing(_entries.id, _entries.object), ...);
        CloseForRead(_file);
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

    virtual void OpenFileForRead(const std::string& _fileToSerialize) = 0;

    virtual void CloseForRead(const std::string& _fileToSerialize) = 0;

    virtual void OpenFileForWrite(const std::string& _fileToSerialize) = 0;

    virtual void CloseForWrite(const std::string& _fileToSerialize) = 0;


    std::ifstream m_Instream;

    std::ofstream m_OutStream;

private:    


};

template<typename... T>
void Serializer::Serialize(const T&... _object, const std::string& _fileToSerialize)
{
    PERF_REGION_SCOPED;

    static_assert((... && !std::is_pointer_v<std::remove_reference_t<T>>),
        "you shouldn't serialize a raw ptr");

    OpenFileForWrite(_fileToSerialize);

    (Serializing(reinterpret_cast<const uint8_t*>( &std::as_const(_object)),
        COMPILE_TIME_TYPE_KEY(std::remove_cv_t<std::remove_reference_t<T>>)), ...);

    CloseForWrite(_fileToSerialize);
}

template<typename... T>
void Serializer::DeSerialize(T*... _object, const std::string& _fileToSerialize)
{
    PERF_REGION_SCOPED;

    static_assert((... && !std::is_pointer_v<T>),
        "You should deSerialize only to a value type, not a pointer-to-pointer");

    OpenFileForRead(_fileToSerialize);

    (DeSerializing(reinterpret_cast<uint8_t*>(_object), COMPILE_TIME_TYPE_KEY(T)), ...);

    CloseForRead(_fileToSerialize);
}

END_PCCORE
