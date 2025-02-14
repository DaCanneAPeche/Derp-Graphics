#include "glm/glm.hpp"

namespace dg
{
  struct UniformBufferObject
  {
    alignas(16) glm::mat2 screenTransform;
  };
}
