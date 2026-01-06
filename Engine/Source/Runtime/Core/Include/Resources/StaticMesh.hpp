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



    struct SubMesh 
    {
        uint32_t VertexOffSet;
        uint32_t VerticiesCount;
        uint32_t IndexOffset;
        uint32_t IndiciesCount;
        uint32_t MaterialIndex;
    };

    struct StaticMeshRenderData
    {
        std::vector<StaticMeshVertex> Vertices;
        std::vector<uint32_t> Indices;
        std::vector<SubMesh> SubMeshes;
    };


    class PC_CORE_API StaticMesh : public Resource
    {
    public:

        explicit StaticMesh(std::string _Name, const StaticMeshRenderData& _StaticMeshRenderData);

        explicit StaticMesh(std::string _Name, StaticMeshRenderData&& _StaticMeshRenderData);

        StaticMesh();

        ~StaticMesh() override = default;

        DEFAULT_COPY_MOVE_OPERATIONS(StaticMesh)

        IMP_DYNAMIC_REFLECT()

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

        const StaticMeshRenderData& GetStaticMeshRenderData() const
        {
            return m_StaticMeshRenderData;
        }

        VertexBuffer VBuffer;

        IndexBuffer IBuffer;


    private:
        bool m_HallowCpuAcces = false;

        StaticMeshRenderData m_StaticMeshRenderData;

        MotionCore::Aabb<double> m_Aabb;

        REFLECT(StaticMesh, Resource)
        REFLECT_MEMBER(StaticMesh, m_HallowCpuAcces)
        REFLECT_MEMBER(StaticMesh, m_Aabb)
    };

END_PCCORE
