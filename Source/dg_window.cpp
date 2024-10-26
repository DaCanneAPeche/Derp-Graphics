#include "dg_window.hpp"
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
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_window = glfwCreateWindow(info.width, info.height, info.name.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(m_window, this);
        // glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
        
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

    void Window::frameBufferResizeCallback(GLFWwindow* pGLFWWindow, int width, int height)
    {
        Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(pGLFWWindow));
        window->isResized = true;
        window->info.width = width;
        window->info.height = height;
    }

} /* dg */ 
