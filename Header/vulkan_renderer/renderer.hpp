#pragma once

#include "vulkan_renderer/reflection.hpp"
#include "vulkan_renderer/window.hpp"
#include "vulkan_renderer/pipeline.hpp"
#include "vulkan_renderer/swapchain.hpp"
#include "vulkan_renderer/model.hpp"
#include "vulkan_renderer/texture.hpp"
#include "_vulkan/vulkan_tool_box.hpp"
#include "vulkan_renderer/asset_manager.hpp"
#include "vulkan_renderer/uniform_buffer_object.hpp"
#include "_vulkan/descriptor_set_manager.hpp"
#include "vulkan_renderer/render_pass.hpp"
#include "utils/castable_to.hpp"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

// std
#include <memory>
#include <array>
#include <any>
#include <functional>

namespace dg
{
  struct Frame;

	class Renderer
	{
	public:
		Renderer(const ConfigInfo& configInfo, VulkanToolBox& vulkanToolBox);
		~Renderer() {};

		Renderer(const Renderer &) = delete;
		Renderer &operator=(const Renderer &) = delete;

    void init();
    void clean();

    Frame startFrame();
    void endFrame(Frame& frame);

		void pollEvents() const { glfwPollEvents(); };
		[[nodiscard]] bool shouldWindowClose() const { return window.shouldClose(); };
		void waitIdle() const { m_toolBox.device.waitIdle(); }
    
    template <class T>
    void pushConstant(vk::CommandBuffer& commandBuffer, const T& pushData)
    {
      commandBuffer.pushConstants(m_pipelineLayout,
          vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
          0, sizeof(T), &pushData);
    }

    template <CastableTo<uint32_t> PipelineIdType>
    void bindPipeline(vk::CommandBuffer& commandBuffer, PipelineIdType pipelineId)
    {
      m_pipelines[static_cast<uint32_t>(pipelineId)]->bind(commandBuffer); 

      commandBuffer.bindDescriptorSets(
          vk::PipelineBindPoint::eGraphics,
          m_pipelineLayout,
          0, m_descriptorSetManager.descriptorSets, {}
          );
    }

    template <CastableTo<uint32_t> PipelineIdType>
    void registerPipelineInfo(PipelineIdType id, const ShaderModule& vertexShader,
        const ShaderModule& fragmentShader,
        std::shared_ptr<PipelineConfigInfo> config = nullptr)
    {
      pipelinesInfo.push_back(PipelineInfo {static_cast<uint32_t>(id),
          vertexShader, fragmentShader, config});
    }
		
		void recreateSwapChain(bool pipelinesCreation = true);
    void updateDescriptorSets() { m_descriptorSetManager.update(); }

		Window window;
    std::vector<PipelineInfo> pipelinesInfo;
    RenderPass renderPass;
    std::shared_ptr<dg::SwapChain> swapChain;
    ShaderDescription shaderDescription;
    std::unordered_map<std::string, DescriptorWriter> descriptors = {};

    static const int MAX_TEXTURE_NUMBER = 1000;
    vk::Sampler m_imageSampler;

	private:

		void createPipelineLayout();
		void createPipelines();
		[[nodiscard]] std::unique_ptr<Pipeline> createPipeline(
				ShaderModule& vertShader,
				ShaderModule& fragShader,
				const std::vector<vk::VertexInputBindingDescription>& bindingDescriptions,
				const std::vector<vk::VertexInputAttributeDescription>& attributeDescriptions,
        std::shared_ptr<PipelineConfigInfo> pPipelineConfig
				);
		void createCommandBuffers();
		void freeCommandBuffers();
		[[nodiscard]] std::vector<const char*> getRequestedExtensions() const;
    void createDescriptorSets();
    void createImageSampler();
    void setupImGui();
    void renderImGui(int imageIndex);

		vk::PipelineLayout m_pipelineLayout;
    DescriptorPool m_descriptorPool;
		std::vector<std::unique_ptr<Pipeline>> m_pipelines;
		std::vector<vk::CommandBuffer> m_commandBuffers;
    VulkanToolBox& m_toolBox;
    DescriptorSetManager m_descriptorSetManager;
	};
} /* dg */ 
