#pragma once

#include "dg_window.hpp"
#include "dg_device.hpp"
#include "dg_pipeline.hpp"
#include "dg_swapchain.hpp"
#include "dg_model.hpp"
#include "dg_sprite.hpp"
#include "dg_texture.hpp"

#include <vk_mem_alloc.hpp>

// std
#include <memory>
#include <array>
#include <any>

namespace dg
{

  struct ApplicationInfo {
		const std::string& name;
		std::array<uint8_t, 3> version;
  };

	class Renderer
	{
	public:
		Renderer(const WindowInfo& windowInfo, const ApplicationInfo& appInfo);
		~Renderer ();

		Renderer(const Renderer &) = delete;
		Renderer &operator=(const Renderer &) = delete;

		void draw();
		void pollEvents() const { glfwPollEvents(); };
		[[nodiscard]] bool shouldWindowClose() const { return window.shouldClose(); };
		void waitIdle() const { m_device.device.waitIdle(); }
		
		Window window;
		vk::Instance instance;
		ApplicationInfo applicationInfo;

	private:

		void createInstance();
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
		void initMemoryAllocator() const;
    void createDescriptorSetLayout();
    void createDescriptorPool();
    void createDescriptorSets();

		std::vector<const char*> m_deviceExtensions = {vk::KHRSwapchainExtensionName};
		Device m_device { instance, m_deviceExtensions, window};
		vk::PipelineLayout m_pipelineLayout;
    vk::DescriptorSetLayout m_descriptorSetLayout;
    vk::DescriptorPool m_descriptorPool;
    std::vector<vk::DescriptorSet> m_descriptorSets;
		std::array<std::unique_ptr<Pipeline>, static_cast<uint32_t>(pl::Count)> m_pipelines;
		std::unique_ptr<SwapChain> m_swapChain;
		std::vector<vk::CommandBuffer> m_commandBuffers;
		vk::DispatchLoaderDynamic m_dispatchLoader;
		// std::unique_ptr<Model> m_model;
    std::unique_ptr<Sprite> m_sprite;
		std::shared_ptr<Texture> m_texture;

		// Validation layers
		std::vector<const  char*> m_validationLayers = {"VK_LAYER_KHRONOS_validation"};
		vk::DebugUtilsMessengerEXT m_debugMessenger;
#ifdef NDEBUG
		const bool m_enableValidationLayers = false;
#else
		const bool m_enableValidationLayers = true;
#endif
		[[nodiscard]] bool areValidationLayersSupported() const;
	};
} /* dg */ 
