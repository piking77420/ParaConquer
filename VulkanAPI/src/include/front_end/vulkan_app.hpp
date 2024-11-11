#pragma once

#include <functional>

#include "rendering/render_harware_interface/RHI.hpp"
#include "vulkan_header.h"
#include "front_end/vulkan_harware_wrapper.hpp"
#include "front_end/vulkan_present_chain.hpp"
#include "front_end/vulkan_shader_manager.hpp"
#include "Imgui/imgui_impl_vulkan.h"

namespace Vulkan
{
	class VulkanApp : public PC_CORE::RHI
	{
	public:
		uint32_t GetCurrentImage() const override;

		VULKAN_API void BeginRender(PC_CORE::CommandPoolHandle _commandBuffer) override;
		
		VULKAN_API void EndRender() override;

		VULKAN_API VulkanApp(const VulkanAppCreateInfo& vulkanMainCreateInfo);

		VULKAN_API ~VulkanApp() override;

		static VULKAN_API ImGui_ImplVulkan_InitInfo GetImGuiInitInfo(PC_CORE::DescriptorPoolHandle* _descriptorPoolHandle);

		VULKAN_API void WaitForAquireImage() override;
		
		VULKAN_API void SwapBuffers(PC_CORE::CommandBufferHandle* _commandBuffers, uint32_t _commandBufferCount) override;

		VULKAN_API void WaitDevice() override;

		VULKAN_API void RecreateSwapChain(void* _glfwWindowptr , uint32_t _newWidht, uint32_t _newHeight) override;

		VULKAN_API PC_CORE::ImageViewHandle GetSwapChainImage(uint32_t imageIndex) override;
		
		VULKAN_API void PushConstants(PC_CORE::CommandBufferHandle _commandBuffer, const std::string& _shaderProgramName,
			const std::string& _pushConstantName, const void* _data, uint32_t _size) override;
#pragma region Shader
		VULKAN_API void BindShaderProgram(PC_CORE::CommandBufferHandle _commandBuffer, const std::string& _shaderProgramName) override;
		
		VULKAN_API void CreateShader(const PC_CORE::ProgramShaderCreateInfo& programShaderCreateInfo, const std::vector<PC_CORE::ShaderSourcePath>& _shaderSource) override;

		VULKAN_API void DestroyImageView(PC_CORE::ImageViewHandle _imageView) override;
		
		VULKAN_API bool DestroyShader(const std::string& _shaderProgramName) override;

#pragma endregion Shader
		
		VULKAN_API PC_CORE::GPUBufferHandle BufferData(size_t _size, const void* _data, PC_CORE::GPU_BUFFER_USAGE _usage) override;

		VULKAN_API PC_CORE::GPUBufferHandle BufferData(size_t _size, PC_CORE::GPU_BUFFER_USAGE _usage) override;

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

		VULKAN_API void AllocDescriptorSet(PC_CORE::DescriptorSetHandle* descriptorSets, uint32_t _descriptorSetCount,
			PC_CORE::DescriptorPoolHandle _descriptorPoolHandle,
			PC_CORE::DescriptorSetLayoutHandle _descriptorSetLayoutHandle) override;

		VULKAN_API void AllocDescriptorSet(const std::string& _shaderName, uint32_t _descriptorSetLayout, PC_CORE::DescriptorSetHandle* _descriptorSet, uint32_t _descriptorSetCount) override;

		VULKAN_API void FreeDescriptorSet(const std::string& _shaderName, PC_CORE::DescriptorSetHandle* _descriptorSet,
			uint32_t _descriptorSetCount) override;

		VULKAN_API void UpdateDescriptorSet(uint32_t _descriptorWriteCount, PC_CORE::DescriptorWriteSet* _descriptorWrite) override;

		VULKAN_API void BindDescriptorSet(PC_CORE::CommandBufferHandle _commandBuffer, const std::string& _shaderProgramName,
			uint32_t _firstSet, uint32_t _descriptorSetCount, const PC_CORE::DescriptorSetHandle* _pDescriptorSets,
			uint32_t _dynamicOffsetCount, const uint32_t* _pDynamicOffsets) override;

#pragma endregion DescriptorSetLayout

#pragma region Image
		
		VULKAN_API void GenerateMimpMap(PC_CORE::ImageHandle _image, PC_CORE::RHIFormat _imageFormat, int32_t _texWidth,
			int32_t _texHeight, uint32_t _mipLevels) override;

		VULKAN_API void UploadData2D(PC_CORE::ImageHandle _imageHandle, uint32_t _mipLevel,
	PC_CORE::TextureAspect _textureAspect, const void* _data, uint32_t _width, uint32_t _height, PC_CORE::Channel _channel) override;
		
		VULKAN_API PC_CORE::SamplerHandle CreateSampler(const PC_CORE::SamplerCreateInfo& _samplerCreateInfo) override;

		VULKAN_API void DestroySampler(PC_CORE::SamplerHandle _samplerHandle) override;

		VULKAN_API void CreateTexture(const PC_CORE::CreateTextureInfo& _createTextureInfo,
			PC_CORE::ImageHandle* _outImageHandle,  PC_CORE::ImageViewHandle* _outImageView) override;

		VULKAN_API bool DestroyTexture(PC_CORE::ImageHandle _imageHandle) override;

#pragma endregion Image

#pragma region Command

		VULKAN_API void CopyBufferToImage(PC_CORE::GPUBufferHandle _buffer, PC_CORE::ImageHandle _image, const PC_CORE::CopyBufferImageInfo& _copyBufferImageInfo) override;

		VULKAN_API void CopyBuffer(PC_CORE::GPUBufferHandle _bufferSrc, PC_CORE::GPUBufferHandle _bufferDst, size_t _srcOffset, size_t _dstOffset, size_t _size) override;

		VULKAN_API void TransitionImageLayout(PC_CORE::ImageHandle _imageHandle, PC_CORE::TextureAspect _imageAspectFlagBits,uint32_t _mipLevel , PC_CORE::ImageLayout _InitialLayout, PC_CORE::ImageLayout _finalLayout) override;
		
#pragma endregion Command

#pragma region RenderPass

		VULKAN_API PC_CORE::RenderPassHandle CreateRenderPass(const PC_CORE::RenderPassCreateInfo& _renderPassCreateInfo) override;

		VULKAN_API void DestroyRenderPass(PC_CORE::RenderPassHandle _renderPassHandle) override;

		VULKAN_API void BeginRenderPass(PC_CORE::CommandBuffer _commandBuffer, PC_CORE::RenderPassHandle _renderPassHandle,
		const PC_CORE::BeginRenderPassInfo& _renderPassInfo) override;
		
		VULKAN_API void EndRenderPass(PC_CORE::CommandBuffer _commandBuffer) override;

#pragma endregion RenderPass

#pragma region Framebuffer
		
		PC_CORE::FrameBufferHandle CreateFrameBuffer(const PC_CORE::RHIFrameBufferCreateInfo& _RHIFrameBufferCreateInfo) override;
		
		void DestroyFrameBuffer(PC_CORE::FrameBufferHandle _frameBufferHandle) override;
		
#pragma endregion Framebuffer


		VULKAN_API void BeginSwapChainRenderPass(PC_CORE::CommandBuffer _commandBuffer);

		VULKAN_API void EndSwapChainRenderPass(PC_CORE::CommandBuffer _commandBuffer);
		
	private:
		VulkanContext m_VulkanContext;
		
		VulkanHarwareWrapper m_vulkanHardwareWrapper;

		VulkanPresentChain m_vulkanPresentChain;

		VulkanShaderManager m_vulkanShaderManager;
		
		vk::CommandBuffer m_BindCommandBuffer = VK_NULL_HANDLE;
		
		std::stack<std::function<void(VulkanContext*)>> m_DeleteFunction;

		void InitBaseObject();

		void DestroyBaseObject();

		void DestroyBuffersAllocations(VulkanContext* _vulkanContext);

		void DestroyImagesAllocations(VulkanContext* _vulkanContext);
	};



}
