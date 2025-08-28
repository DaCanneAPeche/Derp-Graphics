#pragma once
#include <glm/glm.hpp>
#include "plog/Log.h"

namespace dg
{

	struct Transform2d
	{
		glm::vec2 translation = {0.0f, 0.0f};
		glm::vec2 scaling = {1.0f, 1.0f};
		float rotation = 0;
    float ratio = 1;

		[[nodiscard]] glm::mat2 getMatrix() const
		{
			const float sin = glm::sin(rotation);
			const float cos = glm::cos(rotation);
			glm::mat2 rotationMatrix {{cos, sin}, {-sin, cos}};

			glm::mat2 scaleMatrix = {{scaling.x * ratio, 0.0f}, {0.0f, scaling.y}};
      //LOGD << ratio;
			return rotationMatrix * scaleMatrix;
		}
	};
	
} /* dg */ 
