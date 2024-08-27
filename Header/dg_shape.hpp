#pragma once
#include "dg_model.hpp"

namespace dg
{

	struct ShapeVertex : public Vertex<ShapeVertex>
	{

		ShapeVertex(float x, float y)
		{
			position.x = x;
			position.y = y;
		}
		ShapeVertex(glm::vec2 pos) : position {pos} {}

		glm::vec2 position;

		static constexpr std::vector<vk::VertexInputBindingDescription> _getBindingDescriptions()
		{
			std::vector<vk::VertexInputBindingDescription> bindingDescriptions
			{
				vk::VertexInputBindingDescription(0, sizeof(ShapeVertex), vk::VertexInputRate::eVertex)
			};

			return bindingDescriptions;
		}
		
		static constexpr std::vector<vk::VertexInputAttributeDescription> _getAttributeDescriptions()
		{
			std::vector<vk::VertexInputAttributeDescription> attributeDescriptions
			{
				vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32Sfloat, offsetof(ShapeVertex, position))	
			};

			return attributeDescriptions;
		}
	};

	using Shape = Model<ShapeVertex>;
} /* dg */ 
