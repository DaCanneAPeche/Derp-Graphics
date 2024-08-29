#include "dg_model.hpp"

namespace dg
{
	
	Model::Model(Device& device, const std::vector<Vertex>& vertices,
			const std::vector<uint16_t>& indices) : m_device(device)
	{
		createVertexBuffer(vertices);
		createIndexBuffer(indices);
	}

	Model::~Model()
	{
		gAllocator.destroyBuffer(m_vertexBuffer, m_vertexBufferAllocation);
		gAllocator.destroyBuffer(m_indexBuffer, m_indexBufferAllocation);
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
		m_vertexBufferAllocation = handle.second;

		gAllocator.copyMemoryToAllocation(vertices.data(), m_vertexBufferAllocation, 0, bufferSize);
	}

	void Model::createIndexBuffer(const std::vector<uint16_t>& indices)
	{
		m_indicesCount = static_cast<uint32_t>(indices.size());
		assert(m_indicesCount == m_vertexCount && "Model::createIndexBuffer : you need to provide one index by vertex");

		vk::DeviceSize bufferSize = sizeof(indices[0]) * m_indicesCount;
		vk::BufferCreateInfo bufferInfo(
				{}, bufferSize, vk::BufferUsageFlagBits::eIndexBuffer,
				vk::SharingMode::eExclusive);
		vma::AllocationCreateInfo allocInfo(
				vma::AllocationCreateFlagBits::eHostAccessSequentialWrite,
				vma::MemoryUsage::eAuto);

		auto handle = gAllocator.createBuffer(bufferInfo, allocInfo);
		m_indexBuffer = handle.first;
		m_indexBufferAllocation = handle.second;

		gAllocator.copyMemoryToAllocation(indices.data(), m_indexBufferAllocation, 0, bufferSize);
	}

	void Model::bind(const vk::CommandBuffer& commandBuffer)
	{
		commandBuffer.bindVertexBuffers(0, m_vertexBuffer, vk::DeviceSize {0});
		commandBuffer.bindIndexBuffer(m_indexBuffer, vk::DeviceSize {0}, vk::IndexType::eUint16);
	}
	
	void Model::draw(const vk::CommandBuffer& commandBuffer)
	{
		commandBuffer.drawIndexed(m_indicesCount, 1, 0, 0, 0);
	}
} /* dg */ 
