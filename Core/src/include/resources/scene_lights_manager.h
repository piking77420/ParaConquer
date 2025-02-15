#pragma once

#include "resource.hpp"
#include "low_renderer/uniform_buffer.hpp"
#include "math/matrix2x2.hpp"
#include "math/toolbox_typedef.hpp"
//#include "rendering/buffer/uniform_buffer.hpp"

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
    

class SceneLightsBuffer : public ResourceInterface<SceneLightsBuffer>
{
public:
    
    SceneLightsBuffer();

    ~SceneLightsBuffer() = default;

    void Fecth();

    UniformBuffer uniformBuffer;

    SceneLightData sceneLightData;
};

END_PCCORE