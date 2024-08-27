#pragma once

#include "dg_window.hpp"
#include "dg_device.hpp"
#include "dg_pipeline.hpp"
#include "dg_swapchain.hpp"
#include "dg_shape.hpp"

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
		void pollEvents() { glfwPollEvents(); };
		bool shouldWindowClose() { return window.shouldClose(); };
		
		Window window;
		vk::Instance instance;
		ApplicationInfo applicationInfo;

	private:

		void createInstance();
		void createPipelineLayout();
		void createPipelines();
		template<class V>
		std::unique_ptr<Pipeline<V>> createPipeline(const std::string& vertShaderPath, const std::string& fragShaderPath);
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);
		void loadModels();
		std::vector<const char*> getRequestedExtensions();

		std::vector<const char*> m_deviceExtensions = {vk::KHRSwapchainExtensionName};
		Device m_device { instance, m_deviceExtensions, window};
		vk::PipelineLayout m_pipelineLayout;
		// std::unique_ptr<Pipeline> m_pipeline;
		std::array<std::unique_ptr<Pipeline<ShapeVertex>>, static_cast<uint32_t>(pl::Count)> m_pipelines;
		std::unique_ptr<SwapChain> m_swapChain;
		std::vector<vk::CommandBuffer> m_commandBuffers;
		vk::DispatchLoaderDynamic m_dispatchLoader;
		std::unique_ptr<Shape> m_shape;

		// Validation layers
		std::vector<const  char*> m_validationLayers = {"VK_LAYER_KHRONOS_validation"};
		vk::DebugUtilsMessengerEXT m_debugMessenger;
#ifdef NDEBUG
		const bool m_enableValidationLayers = false;
#else
		const bool m_enableValidationLayers = true;
#endif
		bool areValidationLayersSupported();
	};
	
} /* dg */ 
