#pragma once

#include "CoreHeader.hpp"

#include "LowRenderer/CommandList.hpp"

#include "LowRenderer/RhiDescriptorSet.hpp"
#include <Math/ToolBoxTypeDef.hpp>
#include "ObjectPtr.hpp"
#include "Rendering/Buffer/UniformBuffer.hpp"
#include "Rendering/RenderingTypedef.h"
#include "Resources/GraphicShader.hpp"
#include "Scripting/ScriptingLua.hpp"


BEGIN_PCCORE
    class PC_CORE_API DebugDrawContext
    {
    public:
        DEFAULT_COPY_MOVE_OPERATIONS(DebugDrawContext);

        DebugDrawContext() = default;

        ~DebugDrawContext() = default;

        enum class PrimitiveType
        {
            Sphere,
            WireSphere,
            Box,
            WireBox,
            //Capsule, // TODO
            //WireCapsule,
            Count,
        };

        struct RayDraw
        {
            Tbx::Vector3d p1;
            Tbx::Vector3d dir;
            double Distance;
            Tbx::Vector3f Color;
        };

        struct DrawPrimitive
        {
            Tbx::Vector3d Origin;
            Tbx::Vector3d Euler;
            Tbx::Vector3d Size;
            Tbx::Vector3f Color;
        };

        struct Frustum
        {
            Tbx::Matrix4x4d FrustumToWorld;
            Tbx::Vector3f Color;
            bool IsWired = false;
        };

        void PushRay(const Tbx::Vector3d& _P1, const Tbx::Vector3d& _Dir, const float _Distance, const Tbx::Vector3f _Color);

        void PushBoxGizmo(PrimitiveType _primitiveType,
            const Tbx::Vector3d& _p1, const Tbx::Vector3d& euler, const Tbx::Vector3d& _size,
            Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

        void PushSphereGizmo(PrimitiveType _primitiveType, const Tbx::Vector3d& _p1, float _radius = 0.5f,
            Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

        void PushCapsuleGizmo(PrimitiveType _primitiveType, const Tbx::Vector3d& _p1, const Tbx::Vector3d& euler,
            float _radius, float _height, Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

        void PushFrustum(const Tbx::Matrix4x4d& _FrustumToWorld, bool _IsWired, Tbx::Vector3f _Color = Tbx::Vector3f(1.f, 1.f, 1.f));
        
        void ClearForNextFrame();

        static std::pair<std::vector<Tbx::Vector4f>, std::vector<uint32_t>> GenerateBasePrimitve(PrimitiveType _primitiveType);
 
        static std::string PrimitiveTypeToString(PrimitiveType _primitiveType);

        const std::array<std::vector<DrawPrimitive>, static_cast<size_t>(PrimitiveType::Count)>& DebugDrawPrimitives()
        {
            return m_DebugDrawPrimitives;
        }

        const std::vector<Frustum>& DebugDrawFrustums()
        {
            return m_Frustums;
        }

    private:
        std::vector<RayDraw> m_RayDraws;

        std::array<std::vector<DrawPrimitive>, static_cast<size_t>(PrimitiveType::Count)> m_DebugDrawPrimitives;

        std::vector<Frustum> m_Frustums;
    };


END_PCCORE
