#pragma once
#include "Resource.hpp"
#include "ObjectPtr.hpp"
#include "Mesh.hpp"
#include "Rendering/Material.hpp"

namespace PC_CORE::RHI
{
    class ResourceUpdateBranch;
}

BEGIN_PCCORE
    struct StaticMeshVertex
    {
        Tbx::Vector3f Position;
        Tbx::Vector3f Normal;
        Tbx::Vector3f Tangent;
        Tbx::Vector2f Uv;

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
                    .Format = RhiFormat::R32G32B32Sfloat,
                    .Offset = offsetof(StaticMeshVertex, Tangent)  
                },
                {
                    .Binding = _binding,
                    .Location = 3,
                    .Format = RhiFormat::R32G32Sfloat,
                    .Offset = offsetof(StaticMeshVertex, Uv)
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

        MotionCore::Aabb<double> AABB;
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

        explicit StaticMesh(std::string _Name, const PC_CORE::ObjectPtr<Resource>& SharedMesh, SubMesh subMesh);

        explicit StaticMesh(std::string _Name, const StaticMeshRenderData& _StaticMeshRenderData, RHI::ResourceUpdateBranch* _branch);

        explicit StaticMesh(std::string _Name, StaticMeshRenderData&& _StaticMeshRenderData, RHI::ResourceUpdateBranch* _branch);

        StaticMesh();

        ~StaticMesh() override = default;

        DEFAULT_COPY_MOVE_OPERATIONS(StaticMesh)

        IMP_DYNAMIC_REFLECT()

        void AfterSerialize(Serializer* _serializer) const override;

        void AfterDeSerialize(Serializer* _serializer) override;

        StaticMesh& SetAABB(const MotionCore::Aabb<double>& _AABB)
        {
            m_Aabb = _AABB;
            return *this;
        }

        StaticMesh& SetBaseMaterials(const std::vector<ObjectPtr<Rendering::Material>>& _Material);


        const VertexBuffer& GetVertexBuffer() const
        {
            return m_VertexBuffer;
        }

        const IndexBuffer& GetIndexBuffer() const
        {
            return m_IndexBuffer;
        }

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

        const std::vector<WeakObjectPtr<PC_CORE::Rendering::Material>>& GetBaseMaterial() const;

    private:
        VertexBuffer m_VertexBuffer;

        IndexBuffer m_IndexBuffer;

        StaticMeshRenderData m_StaticMeshRenderData;

        MotionCore::Aabb<double> m_Aabb;

        bool m_HallowCpuAcces = false;

        std::vector<WeakObjectPtr<PC_CORE::Rendering::Material>> m_BaseMaterials;

        void InitFromRenderData(const StaticMeshRenderData& _StaticMeshRenderData, RHI::ResourceUpdateBranch* _Branch);

        REFLECT(StaticMesh, Resource)
        REFLECT_MEMBER(StaticMesh, m_HallowCpuAcces)
        REFLECT_MEMBER(StaticMesh, m_Aabb)
    };

END_PCCORE
