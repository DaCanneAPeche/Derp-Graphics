#pragma once
#include "glm/glm.hpp"

namespace dg
{

	struct PushConstant {
		glm::mat2 transform {1.f};
		alignas(8) glm::vec2 offset {.0f, .0f};
	};
	
} /* dg */ 
