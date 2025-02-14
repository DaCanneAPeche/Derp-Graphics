#include "vulkan_renderer/window.hpp"
#include <plog/Log.h>

#include <stdexcept>

namespace dg
{
    
    Window::Window(const WindowInfo& windowInfo) : info {windowInfo}
    {
        init();
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    void Window::init()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        m_window = glfwCreateWindow(info.width, info.height, info.name.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(m_window, this);

        glfwSetFramebufferSizeCallback(m_window, _frameBufferResizeCallback);
        glfwSetKeyCallback(m_window, _keyInputCallback);
        glfwSetCursorPosCallback(m_window, _mouseMoveCallback);
        glfwSetMouseButtonCallback(m_window, _mouseButtonCallback);
        
        PLOG_INFO << "Window created";
    }

    bool Window::shouldClose() const
    {
        return glfwWindowShouldClose(m_window);
    }

    void Window::createWindowSurface(vk::Instance& instance, VkSurfaceKHR* pSurface)
    {

        if (glfwCreateWindowSurface(instance, m_window, nullptr, pSurface) != VK_SUCCESS)
            throw std::runtime_error("Failed to create window surface");

        PLOG_INFO << "Window surface created";
    }

    vk::Extent2D Window::getVkExtent() const
    {
        return {
            static_cast<uint32_t>(info.width),
            static_cast<uint32_t>(info.height),
        };
    }

    void Window::_frameBufferResizeCallback(GLFWwindow* pGLFWWindow, int width, int height)
    {
        Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(pGLFWWindow));
        window->isResized = true;
        window->info.width = width;
        window->info.height = height;

        window->resizeCallback(pGLFWWindow, width, height);
    }

    void Window::_keyInputCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods)
    {
      static_cast<Window*>(glfwGetWindowUserPointer(pWindow))
        ->keyInputCallback(pWindow, key, scancode, action, mods);
    }

    void Window::_mouseMoveCallback(GLFWwindow* pWindow, double xPos, double yPos)
    {
      static_cast<Window*>(glfwGetWindowUserPointer(pWindow))
        ->mouseMoveCallback(pWindow, xPos, yPos);
    }

    void Window::_mouseButtonCallback(GLFWwindow* pWindow, int button, int action, int mods)
    {
      static_cast<Window*>(glfwGetWindowUserPointer(pWindow))
        ->mouseButtonCallback(pWindow, button, action, mods);
    }

} /* dg */ 
