#pragma once

#include <vector>

#include "core_header.hpp"
#include "rendering/render_harware_interface/vertex.hpp"

BEGIN_PCCORE
    class Skydom
{
public:
    //VulkanVertexBuffer m_VulkanVertexBuffer;

    uint32_t nbrOfVerticies = -1;
    
    PC_CORE_API void Generate(int32_t _nbrOfRows, int32_t _nbrOfCols, float radius);

    PC_CORE_API void Destroy();

private:
    Tbx::Vector2f GetTextCoord(float _nx, float _ny);
};

END_PCCORE