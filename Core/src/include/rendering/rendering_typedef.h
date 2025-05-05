#pragma once

#include "core_header.hpp"
#include "gbuffers.hpp"
#include "low_renderer/frame_buffer.hpp"
#include "math/matrix_transformation.hpp"
#include "resources/texture.hpp"
#include "low_renderer/descriptor_set.hpp"

BEGIN_PCCORE



    struct LowLevelCamera
    {
        Tbx::Vector3f position;
        Tbx::Vector3f front;
        Tbx::Vector3f up;

        float aspect; 
        float fov;
        float near;
        float far;
        bool isOrthographic;
    };

    struct RenderingContext
    {
        LowLevelCamera lowLevelCamera;
        float deltaTime;
        float time;
        
        std::shared_ptr<FrameBuffer> gbufferFrameBuffer;
        std::shared_ptr<FrameBuffer> finalImageFrameBuffer;
        
        PC_CORE::ShaderProgramDescriptorSets* viewPortDescriptorSet;
        
        Tbx::Vector2ui renderingContextSize;
    };


END_PCCORE