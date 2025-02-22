#pragma once

#include "_vulkan/buffer.hpp"

// glm
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include "glm/glm.hpp"

// std
#include <vector>
#include <memory>

namespace dg
{

	struct Vertex
	{
		glm::vec2 position;
    glm::vec2 textureCoordinates;

		[[nodiscard]] static constexpr std::vector<vk::VertexInputBindingDescription> getBindingDescriptions()
		{
			std::vector<vk::VertexInputBindingDescription> bindingDescriptions
			{
				vk::VertexInputBindingDescription(0, sizeof(Vertex), vk::VertexInputRate::eVertex)
			};

			return bindingDescriptions;
		}
		
		[[nodiscard]] static constexpr std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions()
		{
			std::vector<vk::VertexInputAttributeDescription> attributeDescriptions
			{
				vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, position)),
				vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, textureCoordinates))
			};

			return attributeDescriptions;
		}
	};

	class Model
	{
	public:

		Model(const std::vector<Vertex>& vertices,
        const std::vector<uint16_t>& indices,
        VulkanToolBox& vulkanToolBox);
		~Model();
		
		Model(const Model &) = delete;
		Model &operator=(const Model &) = delete;

		void bind(const vk::CommandBuffer& commandBuffer) const;
		void draw(const vk::CommandBuffer& commandBuffer) const;

	private:
		void createVertexBuffer(const std::vector<Vertex>& vertices);
		void createIndexBuffer(const std::vector<uint16_t>& indices);

		uint32_t m_vertexCount;
		uint32_t m_indicesCount;
		std::unique_ptr<SpecialisedBuffer<Vertex>> m_vertexBuffer;
		std::unique_ptr<SpecialisedBuffer<uint16_t>> m_indexBuffer;
		bool m_hasIndices = false;
    VulkanToolBox& m_toolBox;
	};
	
} /* dg */ 
