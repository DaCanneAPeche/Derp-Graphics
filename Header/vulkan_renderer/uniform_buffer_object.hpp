#include "glm/glm.hpp"

namespace dg
{
  struct UniformBufferObject
  {
    alignas(16) glm::mat2x2 screenTransform;
  };
}
