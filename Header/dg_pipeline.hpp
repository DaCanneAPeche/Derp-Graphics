#pragma once

#include "vulkan/vulkan.hpp"
#include "dg_device.hpp"

namespace dg
{

	struct PipelineConfigInfo
	{
		vk::PipelineViewportStateCreateInfo viewportInfo;
		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		vk::PipelineRasterizationStateCreateInfo rasterizationInfo;
		vk::PipelineMultisampleStateCreateInfo multisampleInfo;
		vk::PipelineColorBlendAttachmentState colorBlendAttachment;
		vk::PipelineColorBlendStateCreateInfo colorBlendInfo;
		vk::PipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		vk::PipelineDynamicStateCreateInfo dynamicStateInfo;
		vk::PipelineLayout pipelineLayout = nullptr;
		vk::RenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};


	class Pipeline
	{

	public:
		Pipeline(Device& device);
		~Pipeline();

		Pipeline(const Pipeline &) = delete;
		Pipeline& operator=(const Pipeline &) = delete;

	private:

		void createGraphicsPipeline(const std::string& vertShaderPath,
				const std::string& fragShaderPath,
				const PipelineConfigInfo& configInfo);

		vk::ShaderModule createShaderModule(const std::vector<char>& code);
		
		Device& m_device;
		VkPipeline m_graphicsPipeline;
		vk::ShaderModule m_vertShaderModule;
		vk::ShaderModule m_fragShaderModule;

	};
	
} /* dg */ 
