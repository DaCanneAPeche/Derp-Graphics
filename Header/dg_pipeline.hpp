#pragma once

#include "vulkan/vulkan.hpp"

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
			Pipeline();
			~Pipeline();

			Pipeline(const Pipeline &) = delete;
			Pipeline& operator=(const Pipeline &) = delete;


		private:

	};
	
} /* dg */ 
