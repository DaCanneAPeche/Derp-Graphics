#pragma once

#include "vulkan_renderer/window.hpp"
#include "vulkan_renderer/pipeline.hpp"
#include "vulkan_renderer/swapchain.hpp"
#include "vulkan_renderer/model.hpp"
#include "vulkan_renderer/sprite.hpp"
#include "vulkan_renderer/texture.hpp"
#include "_vulkan/vulkan_tool_box.hpp"

#include <vk_mem_alloc.hpp>

// std
#include <memory>
#include <array>
#include <any>
#include <functional>

namespace dg
{

	class Renderer
	{
	public:
		Renderer(const WindowInfo& windowInfo, VulkanToolBox& vulkanToolBox);
		~Renderer() {};

		Renderer(const Renderer &) = delete;
		Renderer &operator=(const Renderer &) = delete;

    void init();
    void clean();

    std::function<void(vk::CommandBuffer&, vk::PipelineLayout&)> externalRendering;
		void draw();
		void pollEvents() const { glfwPollEvents(); };
		[[nodiscard]] bool shouldWindowClose() const { return window.shouldClose(); };
		void waitIdle() const { m_toolBox.device.waitIdle(); }
		
		Window window;

	private:

		void createPipelineLayout();
		void createPipelines();
		[[nodiscard]] std::unique_ptr<Pipeline> createPipeline(
				const std::string& vertShaderPath,
				const std::string& fragShaderPath,
				const std::vector<vk::VertexInputBindingDescription>& bindingDescriptions,
				const std::vector<vk::VertexInputAttributeDescription>& attributeDescriptions
				);
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);
		void loadModels();
		[[nodiscard]] std::vector<const char*> getRequestedExtensions() const;
    void createDescriptorSetLayout();
    void createDescriptorPool();
    void createDescriptorSets();
    void createImageSampler();

		vk::PipelineLayout m_pipelineLayout;
    vk::DescriptorSetLayout m_descriptorSetLayout;
    vk::DescriptorPool m_descriptorPool;
    std::vector<vk::DescriptorSet> m_descriptorSets;
		std::array<std::unique_ptr<Pipeline>, static_cast<uint32_t>(pl::Count)> m_pipelines;
		std::unique_ptr<SwapChain> m_swapChain;
		std::vector<vk::CommandBuffer> m_commandBuffers;
    std::unique_ptr<Sprite> m_sprite;
		std::shared_ptr<Texture> m_texture;
    vk::Sampler m_imageSampler;
    VulkanToolBox& m_toolBox;
	};
} /* dg */ 
