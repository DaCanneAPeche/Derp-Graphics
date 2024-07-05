#pragma once

#include "dg_window.hpp"
#include "dg_device.hpp"

#include <memory>
#include <array>

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
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);

		std::vector<const char*> m_requestedExtensions = {vk::KHRSwapchainExtensionName};
		Device m_device { instance, m_requestedExtensions};
		vk::PipelineLayout m_pipelineLayout;
	};
	
} /* dg */ 
