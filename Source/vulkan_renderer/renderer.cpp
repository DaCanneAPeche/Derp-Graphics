#include "vulkan_renderer/renderer.hpp"
#include "utils/file.hpp"
#include "vulkan_renderer/push_constant.hpp"
#include "core/transform2d.hpp"
#include "core/timer.hpp"
#include "vulkan_renderer/frame.hpp"

// vulkan
#include "vulkan/vulkan.hpp"

#include "glm/gtc/constants.hpp"
#include <glm/ext/matrix_clip_space.hpp>

#include <plog/Log.h>

// std
#include <cstring>

static void imgui_check_vk_result(VkResult err)
{
    if (err == 0)
        return;

    LOG_FATAL << "[ImGui vulkan] Error : VkResult = " << err;
    if (err < 0)
      throw std::runtime_error("Imgui vulkan error, see log files");
}

namespace dg
{
  Renderer::Renderer(const WindowInfo& windowInfo, VulkanToolBox& vulkanToolBox)
    : window(windowInfo), m_toolBox(vulkanToolBox), renderPass(vulkanToolBox),
    m_descriptorSetManager(vulkanToolBox) { }

  void Renderer::init()
  {
    createImageSampler();

    recreateSwapChain(false);

    groupDescriptorSets();
    createDescriptorSets();

    createPipelineLayout();
    createCommandBuffers();
    recreateSwapChain();

    setupImGui();
  }

  void Renderer::clean()
  {
    m_descriptorSetManager.clean();

    for (auto& pipeline : m_pipelines)
      pipeline = nullptr;
    swapChain = nullptr;

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    m_toolBox.device.destroy(m_imageSampler);
    m_toolBox.device.destroyDescriptorPool(m_descriptorPool.descriptorPool);
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

  void Renderer::createPipelineLayout()
  {
    vk::PushConstantRange pushConstantRange(
        vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
        0, sizeof(PushConstant)
        );

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo({},
        m_descriptorSetManager.layouts, pushConstantRange);

    m_pipelineLayout = m_toolBox.device.createPipelineLayout(pipelineLayoutInfo);

    PLOG_INFO << "Pipeline layout created";
  }

  std::unique_ptr<Pipeline> Renderer::createPipeline(
      ShaderModule& vertShader,
      ShaderModule& fragShader,
      const std::vector<vk::VertexInputBindingDescription>& bindingDescriptions,
      const std::vector<vk::VertexInputAttributeDescription>& attributeDescriptions,
      std::shared_ptr<PipelineConfigInfo> pPipelineConfig
      )
  {
    PipelineConfigInfo pipelineConfig;
    if (pPipelineConfig == nullptr)
      Pipeline::defaultPipelineConfigInfo(pipelineConfig);
    else pipelineConfig = *pPipelineConfig;

    pipelineConfig.renderPass = renderPass.renderPass;
    pipelineConfig.pipelineLayout = m_pipelineLayout;

    return std::make_unique<Pipeline>(m_toolBox,
        vertShader,
        fragShader,
        pipelineConfig,
        bindingDescriptions,
        attributeDescriptions
        );
  }

  // ugly way to to it but I'm too lazy to change the DescriptorSetManager
  void Renderer::groupDescriptorSets()
  {
    DescriptorSlot emptySlot;
    for (const auto& slot : shaderDescription.descriptorSlots)
    {
      if (slot.set + 1 > m_sets.size()) // Set not in m_sets
      {
        m_sets.resize(slot.set + 1);
      }

      if (slot.binding + 1 > m_sets[slot.set].size()) // Same but with binding
      {
        m_sets[slot.set].resize(slot.binding + 1, emptySlot);
      }

      m_sets[slot.set][slot.binding] = std::cref(slot); 
    }
  }

  void Renderer::createDescriptorSets()
  {
    // For imgui
    m_descriptorPool.addToPool(vk::DescriptorType::eCombinedImageSampler, 1);
    m_descriptorPool.numberOfSets += 1;

    for (const auto& set : m_sets)
    {
      DescriptorSetLayout& setLayout = m_descriptorSetManager.addLayout();
      m_descriptorPool.numberOfSets += 1;

      for (const auto& slotRef : set)
      {
        const DescriptorSlot& slot = slotRef.get();
        setLayout.addBinding(slot.type, vk::ShaderStageFlagBits::eAll, slot.arrayCount);
        m_descriptorPool.addToPool(slot.type, slot.arrayCount);
        descriptors[slot.name] = DescriptorWriter {slot.set, slot.binding,
          slot.uboIndex, &m_descriptorSetManager};
      }
      DescriptorSetLayoutIndex layoutIndex = setLayout.create();
      m_descriptorSetManager.addDescriptor(layoutIndex);
    }

    m_descriptorPool.create(m_toolBox, vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
    m_descriptorSetManager.allocate(m_descriptorPool.descriptorPool);
  }

  void Renderer::createPipelines()
  {

    assert(swapChain != nullptr && "Cannot create pipelines before swapchain");
    assert(m_pipelineLayout != nullptr && "Cannot create pipelines before pipeline layout");

    for (auto& pipelineInfo : pipelinesInfo)
    {
      m_pipelines[static_cast<uint32_t>(pipelineInfo.id)] = createPipeline(
          pipelineInfo.vertexShader,
          pipelineInfo.fragmentShader,
          Vertex::getBindingDescriptions(),
          Vertex::getAttributeDescriptions(),
          pipelineInfo.config
          );
    }

    LOG_INFO << "Pipelines created";
  }

  void Renderer::recreateSwapChain(bool pipelinesCreation)
  {
    vk::Extent2D extent = window.getVkExtent();
    while (extent.width == 0 || extent.height == 0)
    {
      extent = window.getVkExtent();
      glfwWaitEvents();

      LOG_INFO << "Swapchain waiting for correct dimensions";
    }

    m_toolBox.device.waitIdle();

    if (swapChain == nullptr)
    {
      swapChain = std::make_unique<SwapChain>(m_toolBox, extent, renderPass);

      LOG_INFO << "Swapchain created";
    }
    else
    {
      swapChain = std::make_unique<SwapChain>(m_toolBox, extent, renderPass,
          std::move(swapChain));
      if (swapChain->imageCount != m_commandBuffers.size())
      {
        freeCommandBuffers();
        createCommandBuffers();
        // createUniformBuffer();

        LOG_INFO << "Swapchain recreated";
      }
    }

    // TODO: Don't recreate the pipeline if compatible with the new swapChain & renderPass
    if (pipelinesCreation) createPipelines();
  }

  void Renderer::createCommandBuffers()
  {
    assert(swapChain != nullptr && "Renderer::createCommandBuffers : swapchain shall not be null");

    m_commandBuffers.resize(swapChain->imageCount);
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

  Frame Renderer::startFrame()
  {
    m_descriptorSetManager.update();

    uint32_t imageIndex;
    vk::Result result = swapChain->acquireNextImage(imageIndex);
    if (result == vk::Result::eErrorOutOfDateKHR)
    {
      recreateSwapChain();
      return startFrame(); // TODO : handle that case
    }

    if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
      throw std::runtime_error("Failed to acquire swap chain image");

    vk::Extent2D swapchainExtent = swapChain->extent;

    vk::CommandBufferBeginInfo beginInfo;
    m_commandBuffers[imageIndex].begin(beginInfo);

    std::array<vk::ClearValue, 2> clearValues = {
      vk::ClearColorValue(0.01f, 0.01f, 0.01f, 0.01f),
      vk::ClearDepthStencilValue(1.0f, 0)
    };

    vk::RenderPassBeginInfo renderPassInfo(
        renderPass.renderPass,
        swapChain->getFrameBuffer(imageIndex),
        vk::Rect2D({0, 0}, swapchainExtent),
        clearValues
        );

    m_commandBuffers[imageIndex].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

    vk::Viewport viewport(0.0f, 0.0f, swapchainExtent.width, swapchainExtent.height, 0.0f, 1.0f);
    vk::Rect2D scissor({0, 0}, swapchainExtent);

    m_commandBuffers[imageIndex].setViewport(0, viewport);
    m_commandBuffers[imageIndex].setScissor(0, scissor);

    m_commandBuffers[imageIndex].bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
       m_pipelineLayout, 0, m_descriptorSetManager.descriptorSets, {});

    return Frame(*this, m_commandBuffers[imageIndex], imageIndex);
  }

  void Renderer::endFrame(Frame& frame)
  {
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), frame.commandBuffer);

    frame.commandBuffer.endRenderPass();
    frame.commandBuffer.end();

    vk::Result result = swapChain->submitCommandBuffers(frame.commandBuffer,
        frame.imageIndex);

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

  void Renderer::setupImGui()
  {
     // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    ImGui_ImplGlfw_InitForVulkan(window._getWindow(), true);
    ImGui_ImplVulkan_InitInfo initInfo {};
    initInfo.Instance = m_toolBox.instance;
    initInfo.PhysicalDevice = m_toolBox.physicalDevice;
    initInfo.Device = m_toolBox.device;
    initInfo.QueueFamily = m_toolBox.queueFamilyIndices.graphicsFamily.value();
    initInfo.Queue = m_toolBox.graphicsQueue;
    initInfo.PipelineCache = VK_NULL_HANDLE;
    initInfo.DescriptorPool = m_descriptorPool.descriptorPool;
    initInfo.RenderPass = renderPass.renderPass;
    initInfo.Subpass = 0;
    initInfo.MinImageCount = 2;
    initInfo.ImageCount = swapChain->imageCount;
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    // initInfo.Allocator = g_Allocator;
    initInfo.CheckVkResultFn = imgui_check_vk_result;
    ImGui_ImplVulkan_Init(&initInfo);
  }

} /* dg */ 
