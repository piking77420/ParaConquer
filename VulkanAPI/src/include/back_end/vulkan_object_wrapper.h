#pragma once
#include <type_traits>

namespace VK_NP
{
    
    template <typename T>
    class VulkanObjectWrapper
    {
    public:
        T object;

        VulkanObjectWrapper(const T& b) : object(b) {}
    
        VulkanObjectWrapper(const VulkanObjectWrapper& other) : object(other.object) {}
    
        VulkanObjectWrapper(VulkanObjectWrapper&& other) : object(std::move(other.object)) {}

        VulkanObjectWrapper& operator=(const VulkanObjectWrapper& _other)
        {
            object = _other.object;
            if constexpr (std::is_pointer_v<T>)
            {
                _other = nullptr;
            }
            else
            {
                _other = {};
            }
        
            return *this;
        }

        bool operator==(const VulkanObjectWrapper& other) const
        {
            return this->object == other.object; 
        }
    };

    
}

namespace std {
    template <typename T>
    struct hash<VK_NP::VulkanObjectWrapper<T>> {
        size_t operator()(const VK_NP::VulkanObjectWrapper<T>& _object) const {
            size_t hashValue = 0;
            hash_combine(hashValue, *reinterpret_cast<const uint64_t*>(&_object.object));     
            return hashValue;
        }

    private:
        // Hash combine function to combine multiple hash values
        template <typename U>
        void hash_combine(size_t& _seed, const U& _value) const {
            std::hash<U> hasher;
            _seed ^= hasher(_value) + 0x9e3779b9 + (_seed << 6) + (_seed >> 2);
        }
    };
}
