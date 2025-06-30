#include "glm/glm.hpp"

namespace dg
{
  struct UniformBufferObject
  {
    // alignas(16) glm::mat2 screenTransform;
    struct {
      alignas(16) glm::vec2 firstColumn;
      alignas(16) glm::vec2 secondColumn;
    } screenTransform;
  };
}
