#pragma once

#include "resource.hpp"
#include "math/matrix2x2.hpp"
#include "math/toolbox_typedef.hpp"
//#include "rendering/buffer/uniform_buffer.hpp"

BEGIN_PCCORE
class SceneLightsBuffer : public Resource
{
public:

    bool CreateDirLightData(Tbx::Vector3f direction, Tbx::Vector3f color,
        float intensity);
    

    SceneLightsBuffer() = default;

    ~SceneLightsBuffer() = default;


private:
    struct DirLightData
    {
        Tbx::Vector3f direction;
        Tbx::Vector3f color;
        float intensity = 0.f;
    };

    struct PointLightData
    {
        Tbx::Vector3f position;
        Tbx::Vector3f color;
        float intensity = 0.f;
    };

    struct SpothLightData
    {
        Tbx::Vector3f position;
        Tbx::Vector3f direction;
        Tbx::Vector3f color;
        float intensity = 0.f;
    };

    struct SceneLightData
    {
        std::unique_ptr<DirLightData> dirLightData = nullptr;
        std::vector<PointLightData> pointLights;
        std::vector<SpothLightData> spothLightDatas;
    };
    
    SceneLightData m_SceneLightData;
};

END_PCCORE