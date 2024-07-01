#include "dg_renderer.hpp"
#include "vulkan/vulkan.hpp"
#include "dg_logger.hpp"

namespace dg
{

    Renderer::Renderer(const WindowInfo& windowInfo, const ApplicationInfo& appInfo)
        : window(windowInfo), applicationInfo {appInfo}
    {
        createInstance();    
        chooseDevices();
    }

    Renderer::~Renderer()
    {
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
		
    void Renderer::chooseDevices()
    {
        m_physicalDevice = device::choosePhysicalDevice(instance);
        Logger::logPhysicalDevice(m_physicalDevice);
    }

    void Renderer::createPipelineLayout()
    {
        Logger::msgLn("Creating pipeline layout");

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo {};
        pipelineLayoutInfo.sType = vk::StructureType::ePipelineLayoutCreateInfo;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        // createPipelineLayout -> need the device
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
