#pragma once
#include <vulkan/vulkan_core.h>

#include "math/toolbox_typedef.hpp"

#include "core_header.hpp"

BEGIN_PCCORE

struct Vertex
{
    Tbx::Vector3f position;
    Tbx::Vector3f normal;
    Tbx::Vector2f textureCoord;

    bool operator==(const Vertex& other) const {
        return position == other.position && normal == other.normal && textureCoord == other.textureCoord;
    }

    static VkVertexInputBindingDescription GetBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, normal);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, textureCoord);
        
        return attributeDescriptions;
    }
};

END_PCCORE



namespace std {
    using namespace PC_CORE;
    template <>
    struct hash<Vertex> {
        size_t operator()(const Vertex& _vertex) const {
            // Combine the hashes of pos, normal, and texCoord using a hash function
            size_t hashValue = 0;
            hash_combine(hashValue, _vertex.position.x);
            hash_combine(hashValue, _vertex.position.y);
            hash_combine(hashValue, _vertex.position.z);
            hash_combine(hashValue, _vertex.normal.x);
            hash_combine(hashValue, _vertex.normal.y);
            hash_combine(hashValue, _vertex.normal.z);
            hash_combine(hashValue, _vertex.textureCoord.x);
            hash_combine(hashValue, _vertex.textureCoord.y);
            return hashValue;
        }

        // Hash combine function to combine multiple hash values
        template <typename T>
        void hash_combine(size_t& _seed, const T& _value) const {
            std::hash<T> hasher;
            _seed ^= hasher(_value) + 0x9e3779b9 + (_seed << 6) + (_seed >> 2);
        }
    };
}