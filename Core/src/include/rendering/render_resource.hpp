#pragma once

#include "core_header.hpp"

BEGIN_PCCORE


class RenderResource
{
public:
    enum class Type
    {
        Buffer,
        Texture
    };

    RenderResource(Type _type, uint32_t _index) : m_ResourceType(_type), m_ResourceIndex(_index)
    {
        
    }

private:
    Type m_ResourceType;

    uint32_t m_ResourceIndex;
};

END_PCCORE