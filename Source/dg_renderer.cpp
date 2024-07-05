#include "dg_renderer.hpp"
#include "vulkan/vulkan.hpp"
#include "dg_logger.hpp"
#include "dg_memory_allocator.hpp"

namespace dg
{

    Renderer::Renderer(const WindowInfo& windowInfo, const ApplicationInfo& appInfo)
        : window(windowInfo), applicationInfo {appInfo}
    {
        createInstance();    
        m_device.init();
        Logger::logPhysicalDevice(m_device.physical);
        MemoryAllocator::init(m_device.physical, m_device.device, instance);
        createPipelineLayout();
    }

    Renderer::~Renderer()
    {
        m_device.clean();
        instance.destroy(nullptr);
    }

    void Renderer::createInstance()
    {
        Logger::msgLn("Creating vk instance");

        vk::ApplicationInfo appInfo(
                applicationInfo.name.c_str(),
                VK_MAKE_VERSION(applicationInfo.version[0], 
                    applicationInfo.version[1], applicationInfo.version[2]),
                "Derp Graphics",
                VK_MAKE_VERSION(1, 0, 0),
                vk::ApiVersion13
                );


        uint32_t extensionsCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionsCount);
        
        vk::InstanceCreateInfo createInfo(
                vk::InstanceCreateFlags(),
                &appInfo,
                0, nullptr,
                extensionsCount,
                glfwExtensions
                );
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = extensionsCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        createInfo.enabledLayerCount = 0;

        Logger::msg("Required GLFW extensions : ");
        Logger::msgCStringArray(glfwExtensions, extensionsCount);

        instance = vk::createInstance(createInfo, nullptr);

        Logger::msgLn("Vk instance created");
    }
		
    void Renderer::createPipelineLayout()
    {
        Logger::msgLn("Creating pipeline layout");

        vk::PipelineLayoutCreateInfo pipelineInfo
            (vk::PipelineLayoutCreateFlags(), 0,  nullptr, 0, nullptr);

        m_pipelineLayout = m_device.device.createPipelineLayoutUnique(pipelineInfo).get();
    }

    void Renderer::createPipeline()
    {

    }

    void Renderer::recreateSwapChain()
    {

    }

    void Renderer::createCommandBuffers()
    {

    }

    void Renderer::freeCommandBuffers()
    {

    }

    void Renderer::recordCommandBuffer(int imageIndex)
    {

    }

    void Renderer::draw()
    {

    }
    
} /* dg */ 
