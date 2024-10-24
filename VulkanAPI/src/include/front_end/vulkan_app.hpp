#pragma once

#include <functional>

#include "rendering/render_harware_interface/RHI.hpp"
#include "vulkan_header.h"
#include "front_end/vulkan_buffer_map.hpp"
#include "front_end/vulkan_harware_wrapper.hpp"
#include "front_end/vulkan_present_chain.hpp"
#include "front_end/vulkan_shader_manager.hpp"

namespace VK_NP
{
	class VulkanApp : public PC_CORE::RHI
	{
	public:
		uint32_t GetCurrentImage() const override;

		VULKAN_API void BeginRender(PC_CORE::CommandPoolHandle _commandBuffer) override;
		
		VULKAN_API void EndRender() override;

		VULKAN_API VulkanApp(const VulkanAppCreateInfo& vulkanMainCreateInfo);

		VULKAN_API ~VulkanApp() override;
		
		VULKAN_API void WaitForAquireImage() override;
		
		VULKAN_API void SwapBuffers(PC_CORE::CommandBufferHandle* _commandBuffers, uint32_t _commandBufferCount) override;

		VULKAN_API void WaitDevice() override;

		VULKAN_API void RecreateSwapChain(void* _glfwWindowptr , uint32_t _newWidht, uint32_t _newHeight) override;

		VULKAN_API void PushConstants(PC_CORE::CommandBufferHandle _commandBuffer, const std::string& _shaderProgramName,
			const std::string& _pushConstantName, const void* _data, uint32_t _size) override;
#pragma region Shader
		VULKAN_API void BindShaderProgram(PC_CORE::CommandBufferHandle _commandBuffer, const std::string& _shaderProgramName) override;

		VULKAN_API void CreateShader(const PC_CORE::ProgramShaderCreateInfo& programShaderCreateInfo, const std::vector<PC_CORE::ShaderSourcePath>& _shaderSource) override;
		
		VULKAN_API bool DestroyShader(const std::string& _shaderProgramName) override;

#pragma endregion Shader
		
		VULKAN_API PC_CORE::GPUBufferHandle BufferData(PC_CORE::CommandPool* _commandPool, size_t _size, const void* _data, PC_CORE::GPU_BUFFER_USAGE _usage) override;

		VULKAN_API PC_CORE::GPUBufferHandle BufferData(PC_CORE::CommandPool* _commandPool, size_t _size, PC_CORE::GPU_BUFFER_USAGE _usage) override;

		VULKAN_API void MapData(PC_CORE::GPUBufferHandle _gpuBufferHandle, void** _data) override;

		VULKAN_API void UnMapData(PC_CORE::GPUBufferHandle _gpuBufferHandle) override;

		VULKAN_API bool DestroyBuffer(PC_CORE::GPUBufferHandle _handle) override;

		VULKAN_API void BindVertexBuffer(PC_CORE::CommandBufferHandle _commandBuffer,uint32_t _firstBinding, uint32_t _bindingCount, PC_CORE::GPUBufferHandle _handle) override;

		VULKAN_API void BindIndexBuffer(PC_CORE::CommandBufferHandle _commandBuffer, PC_CORE::GPUBufferHandle _handle) override;

#pragma region CommandPool Functions
		VULKAN_API void FreeCommandBuffer(PC_CORE::CommandPoolHandle _commandPoolHandle, PC_CORE::CommandBuffer* _commandBuffer, uint32_t _commandBufferCount) override;
		
		VULKAN_API void CreateCommandPool(const PC_CORE::CommandPoolCreateInfo& _commandPoolCreateInfo,
		                       PC_CORE::CommandPoolHandle* _commandPoolHandle) override;
		
		VULKAN_API void DestroyCommandPool(PC_CORE::CommandPoolHandle _commandPoolHandle) override;
		
		 PC_CORE_API void AllocCommandBuffers(PC_CORE::CommandPoolHandle _commandPoolHandle,
														 PC_CORE::CommandBufferCreateInfo _commandBufferCreateInfo) override;

#pragma endregion CommandPool Functions


#pragma region DynamicState
		void SetViewPort(PC_CORE::CommandBufferHandle _commandBufferHandle,
					const PC_CORE::ViewPort& _viewPort) override;
		
		void SetScissor(PC_CORE::CommandBufferHandle _commandBufferHandle,
			const PC_CORE::ScissorRect& _scissorRect) override;
#pragma endregion DynamicState

#pragma region DrawFunction

		void Draw(PC_CORE::CommandBufferHandle _commandBufferHandle, uint32_t _vertexCount, uint32_t instanceCount,
			uint32_t firstVertex, uint32_t firstInstance) override;

		void DrawIndexed(PC_CORE::CommandBufferHandle _commandBufferHandle, uint32_t _indiciesCount,
			uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffSet, uint32_t firstInstance) override;
		
#pragma endregion DrawFunction

#pragma region DescriptorSetLayout

		VULKAN_API PC_CORE::DescriptorSetLayoutHandle CreateDescriptorSetLayout(const std::vector<PC_CORE::DescriptorLayoutBinding>& _descriptorSetLayouts) override;
		
		VULKAN_API void DestroyDescriptorSetLayout(const PC_CORE::DescriptorSetLayoutHandle& _descriptorSetLayoutHandle) override;

		VULKAN_API PC_CORE::DescriptorPoolHandle CreateDescriptorPool(const PC_CORE::DescriptorPoolSize* desciptorPoolSize,
			uint32_t descriptorCount, uint32_t maxSets) override;
		
		VULKAN_API void DestroyDescriptorPool(PC_CORE::DescriptorPoolHandle _descriptorPoolHandle) override;

		VULKAN_API void AllocDescriptorSet(PC_CORE::DescriptorSet* descriptorSets, uint32_t _descriptorSetCount,
			PC_CORE::DescriptorPoolHandle _descriptorPoolHandle,
			PC_CORE::DescriptorSetLayoutHandle _descriptorSetLayoutHandle) override;

#pragma endregion DescriptorSetLayout
	private:
		VulkanContext m_VulkanContext;
		
		VulkanBufferMap bufferMap;
		
		VulkanHarwareWrapper m_vulkanHardwareWrapper;

		VulkanPresentChain m_vulkanPresentChain;

		VulkanShaderManager m_vulkanShaderManager;
		
		vk::CommandBuffer m_BindCommandBuffer = VK_NULL_HANDLE;
		
		std::stack<std::function<void(VulkanContext*)>> m_DeleteFunction;

		void InitBaseObject();
	};



}
