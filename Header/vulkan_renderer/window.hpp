#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "vulkan/vulkan.hpp"

// std
#include <string>

namespace dg
{

	struct WindowInfo {

		int width;
		int height;
		std::string name;

	};
		
	class Window
	{
	public:
		Window (const WindowInfo& windowInfo);
		~Window ();

		Window(const Window&) = delete;
		Window &operator=(const Window&) = delete;

		[[nodiscard]] bool shouldClose() const;

		[[nodiscard]] vk::Extent2D getVkExtent() const;
		bool isResized = false;
		WindowInfo info;

		void createWindowSurface(vk::Instance& instance, VkSurfaceKHR* pSurface);

	private:

		static void frameBufferResizeCallback(GLFWwindow* pWindow, int width, int height);
		void init();

		GLFWwindow* m_window;

	};


} /* dg */ 
