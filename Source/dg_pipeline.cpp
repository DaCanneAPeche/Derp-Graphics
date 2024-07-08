#include "dg_pipeline.hpp"
#include "dg_file.hpp"

namespace dg
{

	Pipeline::Pipeline(Device& device) : m_device(device)
	{

	}
		
	void Pipeline::defaultPipelineConfigInfo(PipelineConfigInfo& configInfo)
	{
		configInfo.inputAssemblyInfo = vk::PipelineInputAssemblyStateCreateInfo(
				{},
				vk::PrimitiveTopology::eTriangleList,
				vk::False
				);

		configInfo.viewportInfo = vk::PipelineViewportStateCreateInfo(
				{}, 1, {}, 1, {}
				);

		configInfo.rasterizationInfo = vk::PipelineRasterizationStateCreateInfo(
				{},
				vk::False,
				vk::False,
				vk::PolygonMode::eFill,
				vk::CullModeFlagBits::eNone,
				vk::FrontFace::eClockwise,
				vk::False,
				0.0f,
				0.0f,
				0.0f,
				1.0f
				);

		configInfo.multisampleInfo = vk::PipelineMultisampleStateCreateInfo(
				{},
				vk::SampleCountFlagBits::e1,
				vk::False
				);

		configInfo.colorBlendAttachment = vk::PipelineColorBlendAttachmentState(
				vk::False,
				vk::BlendFactor::eOne,
				vk::BlendFactor::eZero,
				vk::BlendOp::eAdd,
				vk::BlendFactor::eOne,
				vk::BlendFactor::eZero,
				vk::BlendOp::eAdd,
				vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
					vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
				);

		configInfo.colorBlendInfo = vk::PipelineColorBlendStateCreateInfo(
				{},
				vk::False,
				vk::LogicOp::eCopy,
				configInfo.colorBlendAttachment,
				{0.0f, 0.0f, 0.0f, 0.0f}
				);

		configInfo.depthStencilInfo = vk::PipelineDepthStencilStateCreateInfo(
				{},
				vk::True,
				vk::True,
				vk::CompareOp::eLess,
				vk::False,
				vk::False,
				{}, {}, 0.0f, 1.0f
				);

		configInfo.dynamicStateEnables = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };

		configInfo.dynamicStateInfo = vk::PipelineDynamicStateCreateInfo(
				{},
				configInfo.dynamicStateEnables
				);
	}

	Pipeline::~Pipeline()
	{

	}

	void Pipeline::createGraphicsPipeline(
			const std::string& vertShaderPath,
			const std::string& fragShaderPath,
			const PipelineConfigInfo& configInfo
			)
	{
    assert(configInfo.pipelineLayout != VK_NULL_HANDLE && 
            "Cannot create graphics pipeline : no pipelineLayout provided in configInfo");
    
    assert(configInfo.renderPass != VK_NULL_HANDLE && 
            "Cannot create graphics pipeline : no renderPass provided in configInfo");

		std::vector<char> vertCode = file::read(vertShaderPath);
		std::vector<char> fragCode = file::read(fragShaderPath);

		m_vertShaderModule = createShaderModule(vertCode);
		m_fragShaderModule = createShaderModule(fragCode);

		vk::PipelineShaderStageCreateInfo shaderStages[2];

		shaderStages[0] = vk::PipelineShaderStageCreateInfo(
				vk::PipelineShaderStageCreateFlags(),
				vk::ShaderStageFlagBits::eVertex,
				m_vertShaderModule,
				"main"
				);
		
		shaderStages[1] = vk::PipelineShaderStageCreateInfo(
				vk::PipelineShaderStageCreateFlags(),
				vk::ShaderStageFlagBits::eFragment,
				m_fragShaderModule,
				"main"
				);

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo;

		vk::GraphicsPipelineCreateInfo pipelineInfo(
				vk::PipelineCreateFlags(),
				vk::ArrayProxyNoTemporaries<const vk::PipelineShaderStageCreateInfo>(shaderStages),
				&vertexInputInfo,
				&configInfo.inputAssemblyInfo,
				nullptr,
				&configInfo.viewportInfo,
				&configInfo.rasterizationInfo,
				&configInfo.multisampleInfo,
				&configInfo.depthStencilInfo,
				&configInfo.colorBlendInfo,
				&configInfo.dynamicStateInfo,
				configInfo.pipelineLayout,
				configInfo.renderPass,
				configInfo.subpass,
				VK_NULL_HANDLE,
				-1
				);

		m_graphicsPipeline = m_device.device.createGraphicsPipeline(VK_NULL_HANDLE, pipelineInfo).value;
	}

	vk::ShaderModule Pipeline::createShaderModule(const std::vector<char>& code)
	{
		std::vector<uint32_t> iCode(code.begin(), code.end());
		vk::ArrayProxy<const uint32_t> codeArray(iCode);

		vk::ShaderModuleCreateInfo createInfo(
				vk::ShaderModuleCreateFlags(),
				codeArray
				);

		return m_device.device.createShaderModule(createInfo);
	}
	
} /* dg */ 
