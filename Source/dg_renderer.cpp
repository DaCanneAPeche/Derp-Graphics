#include "dg_renderer.hpp"
#include "vulkan/vulkan.hpp"
#include "dg_logger.hpp"

#define VMA_IMPLEMENTATION
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
        instance.destroy();
    }

    void Renderer::createInstance()
    {
        Logger::msgLn("Creating vk instance");

        if (m_enableValidationLayers && !areValidationLayersSupported())
            throw std::runtime_error("Validation layers are requested but not available");

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
        
        uint32_t layerCount = 0;
        const char** ppEnabledLayers = nullptr;
       
        if (m_enableValidationLayers) {
            layerCount = static_cast<uint32_t>(m_validationLayers.size());
            ppEnabledLayers = m_validationLayers.data();
        } 

        vk::InstanceCreateInfo createInfo(
                vk::InstanceCreateFlags(),
                &appInfo,
                layerCount, ppEnabledLayers,
                extensionsCount,
                glfwExtensions
                );
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = extensionsCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        createInfo.enabledLayerCount = 0;

        Logger::msg("Required GLFW extensions : ");
        Logger::msgCStringArray(glfwExtensions, extensionsCount);

        instance = vk::createInstance(createInfo);

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
		
    bool Renderer::areValidationLayersSupported()
    {
        std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

        for (const char* layerName : m_validationLayers)
        {
            bool layerFound = true;

            for (const auto& layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) return false;
        }

        return true;
    }
    
} /* dg */ 
