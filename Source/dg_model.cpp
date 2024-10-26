#include "dg_model.hpp"
#include "dg_globals.hpp"

namespace dg
{
	
	Model::Model(const std::vector<Vertex>& vertices,
			const std::vector<uint16_t>& indices)
	{
		createVertexBuffer(vertices);
		createIndexBuffer(indices);
	}

	Model::~Model()
	{
	}
	
	void Model::createVertexBuffer(const std::vector<Vertex>& vertices)
	{
		m_vertexCount = static_cast<uint32_t>(vertices.size());
		assert(m_vertexCount >= 3 &&
        "Model::createVertexBuffer : you need to provide at least 3 vertices");

		vk::DeviceSize vertexSize = sizeof(vertices[0]);
		vk::DeviceSize bufferSize = vertexSize * m_vertexCount;  

		m_vertexBuffer = std::make_unique<Buffer>(vertexSize, m_vertexCount,
        vk::BufferUsageFlagBits::eVertexBuffer |
				vk::BufferUsageFlagBits::eTransferDst,
        vma::AllocationCreateFlagBits::eHostAccessSequentialWrite,
				vma::MemoryUsage::eAuto);
		m_vertexBuffer->write((void*)vertices.data(), bufferSize);
	}

	void Model::createIndexBuffer(const std::vector<uint16_t>& indices)
	{
		m_indicesCount = static_cast<uint32_t>(indices.size());
		m_hasIndices = m_indicesCount > 0;

		if (!m_hasIndices) return;

		vk::DeviceSize indexSize = sizeof(indices[0]);
		vk::DeviceSize bufferSize = indexSize * m_indicesCount;

		m_indexBuffer = std::make_unique<Buffer>(indexSize, m_indicesCount,
        vk::BufferUsageFlagBits::eIndexBuffer |
				vk::BufferUsageFlagBits::eTransferDst,
        vma::AllocationCreateFlagBits::eHostAccessSequentialWrite,
				vma::MemoryUsage::eCpuToGpu);
		m_indexBuffer->write((void*)indices.data(), bufferSize);
	}

	void Model::bind(const vk::CommandBuffer& commandBuffer) const
	{
		commandBuffer.bindVertexBuffers(0, m_vertexBuffer->buffer, vk::DeviceSize {0});

		if (m_hasIndices)
			commandBuffer.bindIndexBuffer(m_indexBuffer->buffer, vk::DeviceSize {0},
          vk::IndexType::eUint16);
	}
	
	void Model::draw(const vk::CommandBuffer& commandBuffer) const
	{
		if (m_hasIndices)
			commandBuffer.drawIndexed(m_indicesCount, 1, 0, 0, 0);
		else
			commandBuffer.draw(m_indicesCount, 1, 0, 0);
	}
} /* dg */ 
