#pragma once

#include "vulkan/vulkan.hpp"
#include "_vulkan/vulkan_tool_box.hpp"
#include <memory>
#include "vulkan_renderer/shader_module.hpp"

namespace dg
{

	// Pipelines, not a enum class to avoid static cast everywhere
  enum class Pl : uint32_t
	{
    sprites = 0,
    outline,

    Count
	};

	struct PipelineConfigInfo
	{
		vk::PipelineViewportStateCreateInfo viewportInfo;
		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		vk::PipelineRasterizationStateCreateInfo rasterizationInfo;
		vk::PipelineMultisampleStateCreateInfo multisampleInfo;
		vk::PipelineColorBlendAttachmentState colorBlendAttachment;
		vk::PipelineColorBlendStateCreateInfo colorBlendInfo;
		vk::PipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<vk::DynamicState> dynamicStateEnables;
		vk::PipelineDynamicStateCreateInfo dynamicStateInfo;
		vk::PipelineLayout pipelineLayout = nullptr;
		vk::RenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

  struct PipelineInfo
  {
    Pl id;
    ShaderModule vertexShader;
    ShaderModule fragmentShader;

    std::shared_ptr<PipelineConfigInfo> config = nullptr;
  };

	class Pipeline
	{

	public:
		Pipeline(VulkanToolBox& toolBox,
				ShaderModule& vertShader,
				ShaderModule& fragShader,
				const PipelineConfigInfo& configInfo,
				const std::vector<vk::VertexInputBindingDescription>& bindingDescriptions,
				const std::vector<vk::VertexInputAttributeDescription>& attributeDescriptions
				);
		~Pipeline();

		Pipeline(const Pipeline &) = delete;
		Pipeline& operator=(const Pipeline &) = delete;

		static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
		void bind(vk::CommandBuffer& commandBuffer);

	private:

		void createGraphicsPipeline(const PipelineConfigInfo& configInfo);

		[[nodiscard]] vk::ShaderModule createShaderModule(const std::vector<char>& code);
		
		VulkanToolBox& m_toolBox;
		VkPipeline m_graphicsPipeline;
		ShaderModule& m_vertShader;
		ShaderModule& m_fragShader;
    vk::ShaderModule m_vertShaderModule;
    vk::ShaderModule m_fragShaderModule;
		std::vector<vk::VertexInputBindingDescription> m_bindingDescriptions;
		std::vector<vk::VertexInputAttributeDescription> m_attributeDescriptions;
	};
	
} /* dg */ 
