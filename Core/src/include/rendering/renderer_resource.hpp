#pragma once

#include "rendering_typedef.h"

BEGIN_PCCORE

class RendererResource
{
public:


    const std::string& GetName();

    RendererResource(RendererResource&& _other) = default;

    RendererResource& operator=(RendererResource&& _other) = default;
    
    RendererResource(std::string&& _resourName);

    RendererResource(const std::string& _resourName);
    
    virtual ~RendererResource() = default;

private:
    std::string m_ResourName;
};

END_PCCORE