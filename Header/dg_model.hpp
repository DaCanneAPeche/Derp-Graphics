#pragma once

#include "dg_device.hpp"
#include "dg_memory_allocator.hpp"

// glm
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"

// std
#include <vector>

namespace dg
{

	template<class T>
	struct Vertex
	{
		static std::vector<vk::VertexInputBindingDescription> getBindingDescriptions()
		{
			return T::_getBindingDescriptions();
		}
		static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions()
		{
			return T::_getAttributeDescriptions();
		}
	};

	template<class V> // Vertex derived class
	class Model
	{
	public:

		Model(Device& device, const std::vector<V>& vertices);
		~Model();
		
		Model(const Model &) = delete;
		Model &operator=(const Model &) = delete;

		void bind(const vk::CommandBuffer& commandBuffer);
		void draw(const vk::CommandBuffer& commandBuffer);

	private:
		void createVertexBuffer(const std::vector<V>& vertices);

		Device& m_device;
		vk::Buffer m_vertexBuffer;
		vma::Allocation m_bufferMemory;
		uint32_t m_vertexCount;
	};
	
} /* dg */ 
