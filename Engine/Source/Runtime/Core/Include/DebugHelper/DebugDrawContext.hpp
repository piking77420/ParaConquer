#pragma once

#include "CoreHeader.hpp"

#include "LowRenderer/CommandList.hpp"

#include "LowRenderer/RhiDescriptorSet.hpp"
#include "Math/ToolboxTypedef.hpp"
#include "ObjectPtr.hpp"
#include "Rendering/Buffer/UniformBuffer.hpp"
#include "Rendering/RenderingTypedef.h"
#include "Resources/GraphicShader.hpp"
#include "Resources/Mesh.hpp"
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
            Box,
            Capsule,
            WireSphere,
            WireBox,
            WireCapsule,
            Count,
        };

        struct RayDraw
        {
            Tbx::Vector3d p1;
            Tbx::Vector3d dir;
            double Distance;
            Tbx::Vector3f Color;
        };

        struct DrawBox
        {
            Tbx::Vector3d Origin;
            Tbx::Vector3d Euler;
            Tbx::Vector3d Size;
            Tbx::Vector3f Color;
            bool isWireFrame;
        };

        struct DrawSphere
        {
            Tbx::Vector3d Origin;
            Tbx::Vector3d Euler;
            Tbx::Vector3d Size;
            Tbx::Vector3f Color;
            bool isWireFrame;
        };


        void PushRay(const Tbx::Vector3d& _P1, const Tbx::Vector3d& _Dir, const float _Distance, const Tbx::Vector3f _Color);

        void PushBoxGizmo(PrimitiveType _primitiveType,
            const Tbx::Vector3d& _p1, const Tbx::Vector3d& euler, const Tbx::Vector3d& _size,
            Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

        void PushSphereGizmo(PrimitiveType _primitiveType, const Tbx::Vector3d& _p1, float _radius = 0.5f,
            Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));

        void PushCapsuleGizmo(PrimitiveType _primitiveType, const Tbx::Vector3d& _p1, const Tbx::Vector3d& euler,
            float _radius, float _height, Tbx::Vector3f _color = Tbx::Vector3f(1.f, 1.f, 1.f));
        
        void ClearForNextFrame();

        const std::vector<RayDraw>& GetRayDraws() const
        {
            return m_RayDraws;
        }

        const std::vector<DrawBox>& GetDrawBoxs() const
        {
            return m_DrawBoxs;
        }

        const std::vector<DrawSphere>& GetDrawSpheres() const
        {
            return m_DrawSpheres;
        }

        static std::pair<std::vector<Tbx::Vector3f>, std::vector<uint32_t>> GenerateBasePrimitve(PrimitiveType _primitiveType);
 
        static std::string PrimitiveTypeToString(PrimitiveType _primitiveType);

    private:
        std::vector<RayDraw> m_RayDraws;

        std::vector<DrawBox> m_DrawBoxs;

        std::vector<DrawSphere> m_DrawSpheres;


    };


END_PCCORE
