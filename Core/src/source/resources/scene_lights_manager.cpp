#include "resources/scene_lights_manager.h"

bool PC_CORE::SceneLightsBuffer::CreateDirLightData(Tbx::Vector3f direction, Tbx::Vector3f color,
        float intensity)
{
    if (m_SceneLightData.dirLightData != nullptr)
    {
        // Cant have more than 1 directionalLight data
        return false;
    }

    m_SceneLightData.dirLightData = std::make_unique<DirLightData>(direction, color, intensity);
}
