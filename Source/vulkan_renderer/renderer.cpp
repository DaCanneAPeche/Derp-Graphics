#include "vulkan_renderer/renderer.hpp"
#include "vulkan_renderer/globals.hpp"
#include "utils/file.hpp"
#include "vulkan_renderer/push_constant.hpp"
#include "core/transform2d.hpp"

// vulkan
#include "vulkan/vulkan.hpp"

#include "glm/gtc/constants.hpp"

#include <plog/Log.h>

// std
#include <cstring>

namespace dg
{
  Renderer::Renderer(const WindowInfo& windowInfo, VulkanToolBox& vulkanToolBox)
    : window(windowInfo), m_toolBox(vulkanToolBox)
  {}

  void Renderer::init()
  {
    createImageSampler();
    m_texture = std::make_shared<Texture>(m_toolBox,
        "./assets/textures/nothing_suspicious.jpeg",
        m_imageSampler);
    loadModels();

    createDescriptorSetLayout();
    createDescriptorPool();
    createDescriptorSets();

    createPipelineLayout();
    recreateSwapChain();
    createCommandBuffers();
  }

  void Renderer::clean()
  {
    for (auto& pipeline : m_pipelines)
      pipeline = nullptr;
    m_swapChain = nullptr;
    m_sprite = nullptr;
    m_texture = nullptr;

    m_toolBox.device.destroyDescriptorPool(m_descriptorPool);
    m_toolBox.device.destroyDescriptorSetLayout(m_descriptorSetLayout);
    m_toolBox.device.destroyPipelineLayout(m_pipelineLayout);
  }

  void Renderer::createImageSampler()
  {
    vk::SamplerCreateInfo samplerInfo(
        {},
        vk::Filter::eNearest,
        vk::Filter::eNearest,
        vk::SamplerMipmapMode::eLinear,
        vk::SamplerAddressMode::eClampToEdge,
        vk::SamplerAddressMode::eClampToEdge,
        vk::SamplerAddressMode::eClampToEdge,
        .0f,
        vk::False,
        m_toolBox.physicalDevice.getProperties().limits.maxSamplerAnisotropy,
        vk::False,
        vk::CompareOp::eAlways,
        .0f,
        .0f,
        vk::BorderColor::eIntOpaqueBlack,
        vk::False);

    m_imageSampler = m_toolBox.device.createSampler(samplerInfo);
  }

  void Renderer::loadModels()
  {
    m_sprite = std::make_unique<Sprite>(m_texture);
  }

  void Renderer::createPipelineLayout()
  {
    vk::PushConstantRange pushConstantRange(
        vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
        0, sizeof(PushConstant)
        );

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo({}, m_descriptorSetLayout,
        pushConstantRange);

    m_pipelineLayout = m_toolBox.device.createPipelineLayout(pipelineLayoutInfo);

    PLOG_INFO << "Pipeline layout created";
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

    return std::make_unique<Pipeline>(m_toolBox,
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
    m_descriptorSetLayout = m_toolBox.device.createDescriptorSetLayout(layoutInfo);
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

    m_descriptorPool = m_toolBox.device.createDescriptorPool(poolInfo);
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
    m_descriptorSets = m_toolBox.device.allocateDescriptorSets(allocInfo);

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

    m_toolBox.device.updateDescriptorSets(descriptorWrites, {});
  }

  void Renderer::createPipelines()
  {

    assert(m_swapChain != nullptr && "Cannot create pipelines before swapchain");
    assert(m_pipelineLayout != nullptr && "Cannot create pipelines before pipeline layout");

    m_pipelines[pl::shapes] = createPipeline(
        "./assets/compiled_shaders/shape.vert.spv",
        "./assets/compiled_shaders/shape.frag.spv",
        Vertex::getBindingDescriptions(),
        Vertex::getAttributeDescriptions()
        );

    LOG_INFO << "Pipelines created";
  }

  void Renderer::recreateSwapChain()
  {
    vk::Extent2D extent = window.getVkExtent();
    while (extent.width == 0 || extent.height == 0)
    {
      extent = window.getVkExtent();
      glfwWaitEvents();

      LOG_INFO << "Swapchain waiting for correct dimensions";
    }

    m_toolBox.device.waitIdle();

    if (m_swapChain == nullptr)
    {
      m_swapChain = std::make_unique<SwapChain>(m_toolBox, extent);

      LOG_INFO << "Swapchain created";
    }
    else
    {
      m_swapChain = std::make_unique<SwapChain>(m_toolBox, extent, std::move(m_swapChain));
      if (m_swapChain->imageCount() != m_commandBuffers.size())
      {
        freeCommandBuffers();
        createCommandBuffers();

      LOG_INFO << "Swapchain recreated";
      }
    }

    // TODO: Don't recreate the pipeline if compatible with the new swapChain & renderPass
    createPipelines();
  }

  void Renderer::createCommandBuffers()
  {
    assert(m_swapChain != nullptr && "Renderer::createCommandBuffers : swapchain shall not be null");

    m_commandBuffers.resize(m_swapChain->imageCount());
    vk::CommandBufferAllocateInfo allocateInfo(
        m_toolBox.commandPool,
        vk::CommandBufferLevel::ePrimary,
        m_commandBuffers.size()
        );

    m_commandBuffers = m_toolBox.device.allocateCommandBuffers(allocateInfo);

    LOG_INFO << "Command buffers created";
  }

  void Renderer::freeCommandBuffers()
  {
    m_toolBox.device.freeCommandBuffers(m_toolBox.commandPool, m_commandBuffers);
    m_commandBuffers.clear();

    LOG_INFO << "Command buffers freed";
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

    // m_sprite->draw(m_commandBuffers[imageIndex], m_pipelineLayout);
    externalRendering(m_commandBuffers[imageIndex], m_pipelineLayout);

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

} /* dg */ 
