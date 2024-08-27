#include "dg_model.hpp"
#include "dg_shape.hpp"

namespace dg
{
	
	template<class V>
	Model<V>::Model(Device& device, const std::vector<V>& vertices) : m_device(device)
	{
		createVertexBuffer(vertices);
	}

	template<class V>
	Model<V>::~Model()
	{
		MemoryAllocator::destroyBuffer(m_vertexBuffer, m_bufferMemory);
	}

	template<class V>
	void Model<V>::createVertexBuffer(const std::vector<V>& vertices)
	{
		m_vertexCount = static_cast<uint32_t>(vertices.size());
		assert(m_vertexCount >= 3 && "Model::createVertexBuffer : you need to provide at least 3 vertices");

		vk::DeviceSize bufferSize = sizeof(vertices[0]) * m_vertexCount;  

		vk::BufferCreateInfo bufferInfo(
				{}, bufferSize, vk::BufferUsageFlagBits::eVertexBuffer,
				vk::SharingMode::eExclusive);
		vma::AllocationCreateInfo allocInfo({}, vma::MemoryUsage::eAuto);

		auto handle = MemoryAllocator::createBuffer(bufferInfo, allocInfo);
		m_vertexBuffer = handle.first;
		m_bufferMemory = handle.second;
	}

	template<class V>
	void Model<V>::bind(const vk::CommandBuffer &commandBuffer)
	{
		commandBuffer.bindVertexBuffers(0, m_vertexBuffer, vk::DeviceSize {0});
	}
	
	template<class V>
	void Model<V>::draw(const vk::CommandBuffer &commandBuffer)
	{
		commandBuffer.draw(m_vertexCount, 1, 0, 0);
	}
	template class Model<ShapeVertex>;

} /* dg */ 
