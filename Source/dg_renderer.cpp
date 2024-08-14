#include "dg_renderer.hpp"
#include "dg_logger.hpp"
#include "dg_globals.hpp"

// vulkan
#define VMA_IMPLEMENTATION
#include "dg_memory_allocator.hpp"
#include "vulkan/vulkan.hpp"

// std

template <class T>
void executeFunctionStack(std::stack<std::function<void(T)>>& stack, T parameter)
{
    while (stack.size() > 0)
    {
        stack.top()(parameter);
        stack.pop();
    }
}

namespace dg
{

    Renderer::Renderer(const WindowInfo& windowInfo, const ApplicationInfo& appInfo)
        : window(windowInfo), applicationInfo {appInfo}
    {
        createInstance();    
        m_device.init();
        Logger::logPhysicalDevice(m_device.physical);

        MemoryAllocator::init(m_device.physical, m_device.device, instance);
        g::deviceCleaning.push(
                [](dg::Device&)
                {
                    MemoryAllocator::clean();
                });

        createPipelineLayout();
        recreateSwapChain();
        createCommandBuffers();
    }

    Renderer::~Renderer()
    {
        m_swapChain->clean();
        
        executeFunctionStack<dg::Device&>(g::deviceCleaning, m_device);
        executeFunctionStack<vk::Instance&>(g::instanceCleaning, instance);
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

        g::instanceCleaning.push(
                [](vk::Instance& instance)
                {
                    instance.destroy();
                });
    }
		
    void Renderer::createPipelineLayout()
    {
        Logger::msgLn("Creating pipeline layout");

        vk::PipelineLayoutCreateInfo pipelineInfo
            (vk::PipelineLayoutCreateFlags(), 0,  nullptr, 0, nullptr);

        m_pipelineLayout = m_device.device.createPipelineLayout(pipelineInfo);
    }

    void Renderer::createPipeline()
    {
        Logger::msgLn("Creating pipeline");

        assert(m_swapChain != nullptr && "Cannot create pipeline before swapchain");
        assert(m_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig {};
        Pipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = m_swapChain->getRenderPass();
        pipelineConfig.pipelineLayout = m_pipelineLayout;
        m_pipeline = std::make_unique<Pipeline>(m_device);
    }

    void Renderer::recreateSwapChain()
    {
        Logger::msg("Swapchain ");

        vk::Extent2D extent = window.getVkExtent();
        while (extent.width == 0 || extent.height == 0)
        {
            Logger::msgLn("waiting for correct dimensions");
            extent = window.getVkExtent();
            glfwWaitEvents();
        }

        m_device.device.waitIdle();

        if (m_swapChain == nullptr)
        {
            Logger::msgLn("creating");
            m_swapChain = std::make_unique<SwapChain>(m_device, extent);
        }
        else
        {
            Logger::msgLn("recreating");
            m_swapChain = std::make_unique<SwapChain>(m_device, extent, std::move(m_swapChain));
            if (m_swapChain->imageCount() != m_commandBuffers.size())
            {
                freeCommandBuffers();
                createCommandBuffers();
            }
        }

        // TODO: Don't recreate the pipeline if compatible with the new swapChain & renderPass
        createPipeline();
    }

    void Renderer::createCommandBuffers()
    {
        assert(m_swapChain != nullptr && "Renderer::createCommandBuffers : swapchain shall not be null");
        Logger::msgLn("Creating command buffers");

        vk::CommandBufferAllocateInfo allocateInfo(
                m_device.commandPool,
                vk::CommandBufferLevel::ePrimary,
                m_commandBuffers.size()
                );

        m_commandBuffers = m_device.device.allocateCommandBuffers(allocateInfo);
    }

    void Renderer::freeCommandBuffers()
    {
        Logger::msgLn("Freeing command buffers");

        m_device.device.freeCommandBuffers(m_device.commandPool, m_commandBuffers);
        m_commandBuffers.clear();
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
