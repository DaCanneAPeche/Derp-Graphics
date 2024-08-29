#pragma once

#include "dg_device.hpp"
#include "dg_globals.hpp"

// glm
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"

// std
#include <vector>

namespace dg
{

	struct Vertex
	{
		glm::vec2 position;

		static constexpr std::vector<vk::VertexInputBindingDescription> getBindingDescriptions()
		{
			std::vector<vk::VertexInputBindingDescription> bindingDescriptions
			{
				vk::VertexInputBindingDescription(0, sizeof(Vertex), vk::VertexInputRate::eVertex)
			};

			return bindingDescriptions;
		}
		
		static constexpr std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions()
		{
			std::vector<vk::VertexInputAttributeDescription> attributeDescriptions
			{
				vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, position))	
			};

			return attributeDescriptions;
		}
	};

	class Model
	{
	public:

		Model(Device& device, const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices = {0, 1, 2});
		~Model();
		
		Model(const Model &) = delete;
		Model &operator=(const Model &) = delete;

		void bind(const vk::CommandBuffer& commandBuffer);
		void draw(const vk::CommandBuffer& commandBuffer);

	private:
		void createVertexBuffer(const std::vector<Vertex>& vertices);
		void createIndexBuffer(const std::vector<uint16_t>& indices);

		Device& m_device;
		vk::Buffer m_vertexBuffer;
		vma::Allocation m_vertexBufferAllocation;
		uint32_t m_vertexCount;
		vk::Buffer m_indexBuffer;
		vma::Allocation m_indexBufferAllocation;
		uint32_t m_indicesCount;
	};
	
} /* dg */ 
