#include "vulkan_renderer/pipeline.hpp"
#include "utils/file.hpp"
#include "vulkan_renderer/model.hpp"

namespace dg
{

	Pipeline::Pipeline(
			VulkanToolBox& toolBox,
			const std::string& vertShaderPath,
			const std::string& fragShaderPath,
			const PipelineConfigInfo& configInfo,
			const std::vector<vk::VertexInputBindingDescription>& bindingDescriptions,
			const std::vector<vk::VertexInputAttributeDescription>& attributeDescriptions
			) : m_toolBox(toolBox), m_bindingDescriptions(bindingDescriptions),
  m_attributeDescriptions(attributeDescriptions)
	{
		createGraphicsPipeline(vertShaderPath, fragShaderPath, configInfo);
	}

	Pipeline::~Pipeline()
	{
		m_toolBox.device.destroyShaderModule(m_vertShaderModule);
		m_toolBox.device.destroyShaderModule(m_fragShaderModule);
		m_toolBox.device.destroyPipeline(m_graphicsPipeline);
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

		std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages 
		{
			vk::PipelineShaderStageCreateInfo(
					vk::PipelineShaderStageCreateFlags(),
					vk::ShaderStageFlagBits::eVertex,
					m_vertShaderModule,
					"main"
					),
			vk::PipelineShaderStageCreateInfo(
					vk::PipelineShaderStageCreateFlags(),
					vk::ShaderStageFlagBits::eFragment,
					m_fragShaderModule,
					"main"
					)
		};

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo(
				{},
				m_bindingDescriptions,
				m_attributeDescriptions
				);

		vk::GraphicsPipelineCreateInfo pipelineInfo(
				{},
				shaderStages,
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

		m_graphicsPipeline = m_toolBox.device.createGraphicsPipeline(VK_NULL_HANDLE, pipelineInfo).value;
	}

	vk::ShaderModule Pipeline::createShaderModule(const std::vector<char>& code)
	{
		// TODO : Fix the vector casting
		std::vector<uint32_t> iCode(code.begin(), code.end());
		const uint32_t* c = reinterpret_cast<const uint32_t*>(code.data());

		vk::ShaderModuleCreateInfo createInfo(
				vk::ShaderModuleCreateFlags(),
				static_cast<uint32_t>(code.size()),
				c
				);

		return m_toolBox.device.createShaderModule(createInfo);
	}
	
	void Pipeline::bind(vk::CommandBuffer& commandBuffer)
	{
		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_graphicsPipeline);
	}
} /* dg */ 

