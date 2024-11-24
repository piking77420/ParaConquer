#include "rendering/renderer_resource.hpp"

const std::string& PC_CORE::RendererResource::GetName()
{
    return m_ResourName;
}

PC_CORE::RendererResource::RendererResource(std::string&& _resourName) : m_ResourName(std::forward<std::string>(_resourName))
{
    
}

PC_CORE::RendererResource::RendererResource(const std::string& _resourName) : m_ResourName(_resourName)
{
}
