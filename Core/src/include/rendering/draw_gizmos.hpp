#pragma once

#include "core_header.hpp"
#include "physics/box_collider.hpp"
#include "physics/sphere_collider.hpp"
#include "resources/mesh.hpp"
#include "vulkan/vulkan_descriptor_pool.hpp"
#include "vulkan/vulkan_descriptor_set_layout.hpp"
#include "vulkan/vulkan_pipeline.hpp"
#include "vulkan/vulkan_pipeline_layout.hpp"
#include "vulkan/vulkan_shader_stage.hpp"
#include "vulkan/vulkan_viewport.hpp"
#include "world/transform.hpp"


BEGIN_PCCORE
    class Scene;
    class Renderer;

class DrawGizmos
{
public:
    void Init(Renderer* renderer);

    void DrawGizmosForward(VkCommandBuffer _commandBuffer, uint32_t _imageIndex, const ViewPort& viewPort);

    void Destroy();

private:
    struct alignas(16) GizmoStruct
    {
        Tbx::Matrix4x4f trs;
        Tbx::Vector3f color;
    };

    static constexpr float zFightingOffSet = 0.001f;
    
    const Renderer* m_Renderer = nullptr;

    VulkanShaderStage m_VulkanShaderStageGizmo;
    
    VulkanPipeline m_GizmoPipeline;
    
    VulkankPipelineLayout m_VkPipelineLayout;

    VulkanDescriptorSetLayout m_DescriptorSetLayout;

    VulkanDescriptorPool m_DescriptorPool;

    std::vector<VkDescriptorSet> m_DescriptorSet;
    
    const VkCommandBuffer* currentCommandBuffer = nullptr;

    uint32_t imageIndex = 0;

    Mesh* sphere = nullptr;

    Mesh* cubeBoid = nullptr;
    
    void InitShader();
    
    void CreateGraphiPipeline();
    
    void DrawColliders();

    void InitDescriptor();

    void DrawSphereCollider(const std::vector<SphereCollider>& sphereColliders , const Scene& scene);

    void DrawBoxCollider(const std::vector<BoxCollider>& boxColliders , const Scene& scene);

    void DrawAABBCollider(const std::vector<BoxCollider>& _boxColliders , const std::vector<SphereCollider>& _sphereColliders, const Scene& scene);

    struct OutAABB
    {
        Tbx::Vector3f center;
        Tbx::Vector3f extent;   
    };
    
    OutAABB  GetAABBFromSphereCollider(const Transform& _transform, const SphereCollider& _sphere);
    OutAABB GetAABBFromBoxCollider(const Transform& _transform, const BoxCollider& _boxCollider);

};

END_PCCORE