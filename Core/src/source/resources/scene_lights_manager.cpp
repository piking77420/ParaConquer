#include "resources/scene_lights_manager.h"


PC_CORE::SceneLightsBuffer::SceneLightsBuffer()
{
    uniformBuffer = UniformBuffer(&sceneLightData, sizeof(sceneLightData));
}

void PC_CORE::SceneLightsBuffer::Fecth()
{
    uniformBuffer.Update(&sceneLightData, sizeof(sceneLightData));
}
