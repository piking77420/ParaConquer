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
    static void Serialize(const fs::path& _fileToSerialize);
    
private:
    struct SerializeContex
    {
        void* currentFile = nullptr;
        void* currentDocument = nullptr;
    };

    
    static void Serializing(uint32_t _TypeKey);

    static void SerializeMember(const Members& _members);

    static void SerializeBegin(const fs::path& _fileToSerialize);
    
    static void SerializeEnd();

    static inline void* currentFile = nullptr;

    static inline size_t currentFileSize = 0;
};

template <typename T>
void Serializer::Serialize(const fs::path& _fileToSerialize)
{
    SerializeBegin(_fileToSerialize);
    SerializeEnd();
}

END_PCCORE
