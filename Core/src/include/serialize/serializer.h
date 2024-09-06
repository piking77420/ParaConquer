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
    static void Serializing(const fs::path& _fileToSerialize, uint32_t _typeKey);

    static void SerializeMember(const Members& _members);
    
    static void SerializeEnd();

    static inline void* currentFile = nullptr;

    static inline size_t currentFileSize = 0;
};

template <typename T>
void Serializer::Serialize(const fs::path& _fileToSerialize)
{
    
}

END_PCCORE
