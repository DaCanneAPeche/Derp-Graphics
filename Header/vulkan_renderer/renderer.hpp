#pragma once

#include "vulkan_renderer/window.hpp"
#include "vulkan_renderer/pipeline.hpp"
#include "vulkan_renderer/swapchain.hpp"
#include "vulkan_renderer/model.hpp"
#include "vulkan_renderer/sprite.hpp"
#include "vulkan_renderer/texture.hpp"
#include "_vulkan/vulkan_tool_box.hpp"

#include <vk_mem_alloc.hpp>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

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

    std::function<void()> externalRendering;
		void draw();
		void pollEvents() const { glfwPollEvents(); };
		[[nodiscard]] bool shouldWindowClose() const { return window.shouldClose(); };
		void waitIdle() const { m_toolBox.device.waitIdle(); }
    
    template <class T>
    void pushConstant(const T& pushData)
    {
      assert(pCurrentCommandBuffer != nullptr);
      pCurrentCommandBuffer->pushConstants(m_pipelineLayout,
          vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
          0, sizeof(T), &pushData);
    }

    void bindPipeline(Pl pipelineId)
    {
      assert(pCurrentCommandBuffer != nullptr);
      m_pipelines[static_cast<uint32_t>(Pl::sprites)]->bind(*pCurrentCommandBuffer); 
      pCurrentCommandBuffer->bindDescriptorSets(
          vk::PipelineBindPoint::eGraphics,
          m_pipelineLayout,
          0, m_descriptorSets, {}
          );
    }
		
		void recreateSwapChain();
		Window window;
    std::vector<PipelineInfo> pipelinesInfo;
    vk::CommandBuffer* pCurrentCommandBuffer = nullptr;

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
		void recordCommandBuffer(int imageIndex);
		void loadModels();
		[[nodiscard]] std::vector<const char*> getRequestedExtensions() const;
    void createDescriptorSetLayout();
    void createDescriptorPool();
    void createDescriptorSets();
    void createImageSampler();
    void setupImGui();
    void renderImGui(int imageIndex);

		vk::PipelineLayout m_pipelineLayout;
    vk::DescriptorSetLayout m_descriptorSetLayout;
    vk::DescriptorPool m_descriptorPool;
    std::vector<vk::DescriptorSet> m_descriptorSets;
		std::array<std::unique_ptr<Pipeline>, static_cast<uint32_t>(Pl::Count)> m_pipelines;
		std::unique_ptr<SwapChain> m_swapChain;
		std::vector<vk::CommandBuffer> m_commandBuffers;
    std::unique_ptr<Sprite> m_sprite;
		std::shared_ptr<Texture> m_texture;
    vk::Sampler m_imageSampler;
    VulkanToolBox& m_toolBox;
	};
} /* dg */ 
