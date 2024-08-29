#include "glm/glm.hpp"

namespace dg
{

	struct PushConstant {
		glm::mat2 transform {1.f};
		alignas(8) glm::vec2 offset {.0f, .0f};
		alignas(16) glm::vec3 color {1.f, 1.f, 1.f};
	};
	
} /* dg */ 
