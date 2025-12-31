#pragma once
#include "Mesh.hpp"
#include "CompactBuffer.hpp"

BEGIN_PCCORE
    struct StaticMeshVertex
    {
        Tbx::Vector3f Position;
        Tbx::Vector3f Normal;
        Tbx::Vector2f Uv;
        Tbx::Vector3f Tangent;

        static constexpr VertexInputBindingDescrition GetVertexBindingDescription(uint32_t _binding);

        static constexpr std::vector<VertexAttributeDescription> GetAttributeDescriptions(uint32_t _binding)
        {
            return
            {
                {
                    .Binding = _binding,
                    .Location = 0,
                    .Format = RhiFormat::R32G32B32Sfloat,
                    .Offset = offsetof(StaticMeshVertex, Position)
                },
                {
                    .Binding = _binding,
                    .Location = 1,
                    .Format = RhiFormat::R32G32B32Sfloat,
                    .Offset = offsetof(StaticMeshVertex, Normal)
                },
                {
                    .Binding = _binding,
                    .Location = 2,
                    .Format = RhiFormat::R32G32Sfloat,
                    .Offset = offsetof(StaticMeshVertex, Uv)
                },
                {
                    .Binding = _binding,
                    .Location = 3,
                    .Format = RhiFormat::R32G32B32Sfloat,
                    .Offset = offsetof(StaticMeshVertex, Tangent)
                }


            };
        }


        REFLECT(StaticMeshVertex)
        REFLECT_MEMBER(StaticMeshVertex, Position)
        REFLECT_MEMBER(StaticMeshVertex, Normal)
        REFLECT_MEMBER(StaticMeshVertex, Uv)
        REFLECT_MEMBER(StaticMeshVertex, Tangent)
    };

    constexpr VertexInputBindingDescrition StaticMeshVertex::GetVertexBindingDescription(const uint32_t _binding)
    {
        return
        {
            .Binding = _binding,
            .Stride = sizeof(StaticMeshVertex),
            .VertexInputRate = VertexInputRate::Vertex
        };
    }


    struct StaticMeshRenderData
    {
        const std::vector<StaticMeshVertex>& Vertices;
        const std::vector<uint32_t>& Indices;
    };


    struct StaticMeshCreateInfo
    {
        std::string Name;
        StaticMeshRenderData StaticMeshRenderData;
        bool HallowCpuAcces;
    };


    class PC_CORE_API StaticMesh : public Resource
    {
    public:
        VertexBuffer VBuffer;

        IndexBuffer IBuffer;

        void AfterSerialize(Serializer* _serializer) const override;

        void AfterDeSerialize(Serializer* _serializer) override;

        const MotionCore::Aabb<double>& GetAabb() const
        {
            return m_Aabb;
        }

        bool GetAlloWCpuAcces() const
        {
            return m_HallowCpuAcces;
        }

        IMP_DYNAMIC_REFLECT()

        DEFAULT_COPY_MOVE_OPERATIONS(StaticMesh)

        explicit StaticMesh(std::string _Name, const StaticMeshRenderData& _StaticMeshRenderData);

        StaticMesh();

        ~StaticMesh() override = default;

    private:
        bool m_HallowCpuAcces = false;

        MotionCore::Aabb<double> m_Aabb;

        REFLECT(StaticMesh, Resource)
        REFLECT_MEMBER(StaticMesh, m_HallowCpuAcces)
        REFLECT_MEMBER(StaticMesh, m_Aabb)
    };

END_PCCORE
