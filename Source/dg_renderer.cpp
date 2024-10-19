#include "dg_renderer.hpp"
#include "dg_logger.hpp"
#include "dg_globals.hpp"
#include "dg_file.hpp"
#include "dg_push_constant.hpp"
#include "dg_transform2d.hpp"

// vulkan
#include "vulkan/vulkan.hpp"

#include "glm/gtc/constants.hpp"

// std
#include <cstring>

namespace dg
{
    Renderer::Renderer(const WindowInfo& windowInfo, const ApplicationInfo& appInfo)
        : window(windowInfo), applicationInfo {appInfo}
    {
      createInstance();   
      m_dispatchLoader = vk::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);
      m_debugMessenger = Logger::createDebugMessenger(instance, m_dispatchLoader);

      m_device.init();
      Logger::logPhysicalDevice(m_device.physical);

      initMemoryAllocator();
      m_texture = std::make_shared<Texture>(m_device, "./assets/textures/nothing_suspicious.jpeg",
          m_device.physical.getProperties().limits.maxSamplerAnisotropy);
      loadModels();

      createDescriptorSetLayout();
      createDescriptorPool();
      createDescriptorSets();
      
      createPipelineLayout();
      recreateSwapChain();
      createCommandBuffers();
    }

    Renderer::~Renderer()
    {
        for (auto& pipeline : m_pipelines)
            pipeline = nullptr;
        m_swapChain = nullptr;
        // m_model = nullptr;
        m_sprite = nullptr;
        m_texture = nullptr;

        m_device.device.destroyDescriptorPool(m_descriptorPool);
        m_device.device.destroyDescriptorSetLayout(m_descriptorSetLayout);

        gAllocator.destroy();

        m_device.device.destroyPipelineLayout(m_pipelineLayout);
        m_device.clean();

        if (m_enableValidationLayers)
            instance.destroyDebugUtilsMessengerEXT(m_debugMessenger, nullptr, m_dispatchLoader);
        instance.destroy();
    }

    void Renderer::loadModels()
    {
        Logger::msgLn("Loading models");

        /*std::vector<Vertex> vertices 
        {
            {{-0.5f, -0.5f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f}, {0.0f, 1.0f}}
        };

        std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};

        m_model = std::make_unique<Model>(m_device, vertices, indices);*/
        m_sprite = std::make_unique<Sprite>(m_device, m_texture);
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


        auto requestedExtensions = getRequestedExtensions();       
        vk::InstanceCreateInfo createInfo(
                vk::InstanceCreateFlags(),
                &appInfo,
                m_validationLayers,
                requestedExtensions
                );

        // Logger::msg("Required GLFW extensions : ");
        // Logger::msgCStringArray(glfwExtensions, extensionsCount);

        instance = vk::createInstance(createInfo);

        Logger::msgLn("Vk instance created");
    }

    void Renderer::initMemoryAllocator() const
    {
        vma::VulkanFunctions vulkanFunctions(
          &vkGetInstanceProcAddr,
          &vkGetDeviceProcAddr
        );

        vma::AllocatorCreateInfo allocatorCreateInfo(
            vma::AllocatorCreateFlagBits::eExtMemoryBudget,
            m_device.physical,
            m_device.device,
            {},
            nullptr,
            nullptr,
            {},
            &vulkanFunctions,
            instance,
            vk::ApiVersion13,
            {}
            );

        gAllocator = vma::createAllocator(allocatorCreateInfo);
    }

    std::vector<const char*> Renderer::getRequestedExtensions() const
    {
          uint32_t glfwExtensionCount = 0;
          const char **glfwExtensions;
          glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

          std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

          if (m_enableValidationLayers) {
            extensions.push_back(vk::EXTDebugUtilsExtensionName);
          }

          return extensions;
    }

    void Renderer::createPipelineLayout()
    {
        Logger::msgLn("Creating pipeline layout");

        vk::PushConstantRange pushConstantRange(
                vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                0, sizeof(PushConstant)
                );

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo({}, m_descriptorSetLayout,
            pushConstantRange);

        m_pipelineLayout = m_device.device.createPipelineLayout(pipelineLayoutInfo);
    }

    std::unique_ptr<Pipeline> Renderer::createPipeline(
            const std::string& vertShaderPath,
            const std::string& fragShaderPath,
            const std::vector<vk::VertexInputBindingDescription>& bindingDescriptions,
            const std::vector<vk::VertexInputAttributeDescription>& attributeDescriptions
            )
    {
        PipelineConfigInfo pipelineConfig {};
        Pipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = m_swapChain->getRenderPass();
        pipelineConfig.pipelineLayout = m_pipelineLayout;

        return std::make_unique<Pipeline>(m_device,
                vertShaderPath,
                fragShaderPath,
                pipelineConfig,
                bindingDescriptions,
                attributeDescriptions
                );
    }
    void Renderer::createDescriptorSetLayout()
    {
      vk::DescriptorSetLayoutBinding samplerLayoutBinding(
          0,
          vk::DescriptorType::eCombinedImageSampler, 1,
          vk::ShaderStageFlagBits::eFragment
          );

      std::array<vk::DescriptorSetLayoutBinding, 1> bindings =
      {
        samplerLayoutBinding
      };

      vk::DescriptorSetLayoutCreateInfo layoutInfo({}, bindings);
      m_descriptorSetLayout = m_device.device.createDescriptorSetLayout(layoutInfo);
    }
    
    void Renderer::createDescriptorPool()
    {
      std::array<vk::DescriptorPoolSize, 1> poolSizes = {
        vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, 
            static_cast<uint32_t>(1))
      };

      vk::DescriptorPoolCreateInfo poolInfo(
          {}, static_cast<uint32_t>(1),
          poolSizes
          );

      m_descriptorPool = m_device.device.createDescriptorPool(poolInfo);
    }
    
    void Renderer::createDescriptorSets()
    {
      std::vector<vk::DescriptorSetLayout> layouts = {
        m_descriptorSetLayout
      };

      vk::DescriptorSetAllocateInfo allocInfo(
          m_descriptorPool,
          layouts);

      m_descriptorSets.resize(1);
      m_descriptorSets = m_device.device.allocateDescriptorSets(allocInfo);

      /*vk::DescriptorImageInfo imageInfo(
          m_texture->sampler,
          m_texture->imageView,
          vk::ImageLayout::eShaderReadOnlyOptimal);*/
      vk::DescriptorImageInfo imageInfo = m_sprite->getImageInfo();

      std::array<vk::WriteDescriptorSet, 1> descriptorWrites = {
        vk::WriteDescriptorSet(
            m_descriptorSets[0],
            0,
            0,
            vk::DescriptorType::eCombinedImageSampler,
            imageInfo
            )
      };

      m_device.device.updateDescriptorSets(descriptorWrites, {});
    }

    void Renderer::createPipelines()
    {
        Logger::msgLn("Creating pipelines");

        assert(m_swapChain != nullptr && "Cannot create pipelines before swapchain");
        assert(m_pipelineLayout != nullptr && "Cannot create pipelines before pipeline layout");

        m_pipelines[pl::shapes] = createPipeline(
                "./assets/compiled_shaders/shape.vert.spv",
                "./assets/compiled_shaders/shape.frag.spv",
                Vertex::getBindingDescriptions(),
                Vertex::getAttributeDescriptions()
                );
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
        createPipelines();
    }

    void Renderer::createCommandBuffers()
    {
        assert(m_swapChain != nullptr && "Renderer::createCommandBuffers : swapchain shall not be null");
        Logger::msgLn("Creating command buffers");

        m_commandBuffers.resize(m_swapChain->imageCount());
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
        vk::Extent2D swapchainExtent = m_swapChain->getSwapChainExtent();

        vk::CommandBufferBeginInfo beginInfo;
        m_commandBuffers[imageIndex].begin(beginInfo);

        std::array<vk::ClearValue, 2> clearValues = {
            vk::ClearColorValue(0.01f, 0.01f, 0.01f, 0.01f),
            vk::ClearDepthStencilValue(1.0f, 0)
        };

        vk::RenderPassBeginInfo renderPassInfo(
                m_swapChain->getRenderPass(),
                m_swapChain->getFrameBuffer(imageIndex),
                vk::Rect2D({0, 0}, swapchainExtent),
                clearValues
                );

        m_commandBuffers[imageIndex].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

        vk::Viewport viewport(0.0f, 0.0f, swapchainExtent.width, swapchainExtent.height, 0.0f, 1.0f);
        vk::Rect2D scissor({0, 0}, swapchainExtent);

        m_commandBuffers[imageIndex].setViewport(0, viewport);
        m_commandBuffers[imageIndex].setScissor(0, scissor);
        
        m_pipelines[pl::shapes]->bind(m_commandBuffers[imageIndex]); 
        m_commandBuffers[imageIndex].bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            m_pipelineLayout,
            0, m_descriptorSets, {}
            );

        m_sprite->transform.rotation += 0.0001f;
        m_sprite->draw(m_commandBuffers[imageIndex], m_pipelineLayout);

        m_commandBuffers[imageIndex].endRenderPass();
        m_commandBuffers[imageIndex].end();
    }

    void Renderer::draw()
    {
        uint32_t imageIndex;
        vk::Result result = m_swapChain->acquireNextImage(imageIndex);
        if (result == vk::Result::eErrorOutOfDateKHR)
        {
            recreateSwapChain();
            return;
        }

        if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
            throw std::runtime_error("Failed to acquire swap chain image");

        recordCommandBuffer(imageIndex);
        result = m_swapChain->submitCommandBuffers(m_commandBuffers[imageIndex], imageIndex);

        if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR
                || window.isResized)
        {
            window.isResized = false;
            recreateSwapChain();
            return;
        }

        if (result != vk::Result::eSuccess)
            throw std::runtime_error("Failed to present swap chain image");
    }
		
    bool Renderer::areValidationLayersSupported() const
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
