#include "dg_window.hpp"
#include "dg_logger.hpp"

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
        Logger::msgLn("Initing window");

        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        m_window = glfwCreateWindow(info.width, info.height, info.name.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(m_window, this);
        // glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
        
        Logger::msgLn("Window inited");
    }

    bool Window::shouldClose()
    {
        return glfwWindowShouldClose(m_window);
    }

    void Window::createWindowSurface(vk::Instance& instance, VkSurfaceKHR* pSurface)
    {
        Logger::msgLn("Creating window surface");

        if (glfwCreateWindowSurface(instance, m_window, nullptr, pSurface) != VK_SUCCESS)
            throw std::runtime_error("Failed to create window surface");

        Logger::msgLn("Window surface created");
    }

    vk::Extent2D Window::getVkExtent()
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
