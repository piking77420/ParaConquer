#pragma once

#include "low_renderer/rhi_uniform_buffer.hpp"
#include "math/matrix2x2.hpp"
#include "math/toolbox_typedef.hpp"
#include "rendering/buffer/uniform_buffer.hpp"

BEGIN_PCCORE


struct ALIGNAS_16 SceneLightData
{
    Tbx::Vector3f direction;
    float padding;
    Tbx::Vector3f color;
    float intensity;
    Tbx::Vector3f ambiant;
    float padding3;
};
    

class SceneLightsBuffer
{
public:
    
    PC_CORE_API SceneLightsBuffer();

    PC_CORE_API ~SceneLightsBuffer() = default;

    PC_CORE_API void Fecth();

    UniformBuffer uniformBuffer;

    SceneLightData sceneLightData;
};

END_PCCORE