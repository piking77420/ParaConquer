#pragma once

#include "core_header.hpp"
#include "enum_class_bit_flag.hpp"
#include "gbuffers.hpp"
#include "low_renderer/frame_buffer.hpp"
#include "math/matrix_transformation.hpp"
#include "resources/texture_2d.hpp"
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

#ifdef WITH_EDITOR
    enum class RenderingContextFlag
    {
        DebugDrawGeometry = 1 << 0  
    };
    

#endif

    struct RenderingContext
    {
        LowLevelCamera lowLevelCamera;
        float deltaTime;
        float time;
        
        std::shared_ptr<FrameBuffer> gbufferFrameBuffer;
        std::shared_ptr<FrameBuffer> finalImageFrameBuffer;
        
        PC_CORE::ShaderProgramDescriptorSets* viewPortDescriptorSet;
        
        Tbx::Vector2ui renderingContextSize;
#ifdef WITH_EDITOR
        BitFlags<RenderingContextFlag> renderingContextFlag;
#endif
    };


END_PCCORE