#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "vulkan/vulkan.hpp"

// std
#include <string>
#include <functional>

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
    GLFWwindow* _getWindow() { return m_window; }

    std::function<void(GLFWwindow*, int, int, int, int)> keyInputCallback;
    std::function<void(GLFWwindow*, double, double)> mouseMoveCallback;
    std::function<void(GLFWwindow*, int, int, int)> mouseButtonCallback;
    std::function<void(GLFWwindow*, int, int)> resizeCallback;

	private:

		static void _frameBufferResizeCallback(GLFWwindow* pWindow, int width, int height);
    static void _keyInputCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods);
    static void _mouseMoveCallback(GLFWwindow* pWindow, double xPos, double yPos);
    static void _mouseButtonCallback(GLFWwindow* pWindow, int button, int action, int mods);
		void init();

		GLFWwindow* m_window;
	};


} /* dg */ 
