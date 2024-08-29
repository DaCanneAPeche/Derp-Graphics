#include "dg_model.hpp"

namespace dg
{
	
	Model::Model(Device& device, const std::vector<Vertex>& vertices) : m_device(device)
	{
		createVertexBuffer(vertices);
	}

	Model::~Model()
	{
		gAllocator.destroyBuffer(m_vertexBuffer, m_bufferAllocation);
	}
	
	void Model::createVertexBuffer(const std::vector<Vertex>& vertices)
	{
		m_vertexCount = static_cast<uint32_t>(vertices.size());
		assert(m_vertexCount >= 3 && "Model::createVertexBuffer : you need to provide at least 3 vertices");

		vk::DeviceSize bufferSize = sizeof(vertices[0]) * m_vertexCount;  

		vk::BufferCreateInfo bufferInfo(
				{}, bufferSize, vk::BufferUsageFlagBits::eVertexBuffer,
				vk::SharingMode::eExclusive);
		vma::AllocationCreateInfo allocInfo(
				vma::AllocationCreateFlagBits::eHostAccessSequentialWrite,
				vma::MemoryUsage::eAuto);

		auto handle = gAllocator.createBuffer(bufferInfo, allocInfo);
		m_vertexBuffer = handle.first;
		m_bufferAllocation = handle.second;
		gAllocator.copyMemoryToAllocation(vertices.data(), m_bufferAllocation, 0, bufferSize);
	}

	void Model::bind(const vk::CommandBuffer &commandBuffer)
	{
		commandBuffer.bindVertexBuffers(0, m_vertexBuffer, vk::DeviceSize {0});
	}
	
	void Model::draw(const vk::CommandBuffer &commandBuffer)
	{
		commandBuffer.draw(m_vertexCount, 1, 0, 0);
	}
} /* dg */ 
