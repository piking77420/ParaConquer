#pragma once


#include "core_header.hpp"
#include "glm/fwd.hpp"
#include "math/toolbox_typedef.hpp"
#include <glm/glm.hpp>


struct alignas(16) UniformBufferObject
{
    glm::mat4 model = glm::mat4(1.f);
    glm::mat4 view = glm::mat4(1.f);
    glm::mat4 proj = glm::mat4(1.f);
};

