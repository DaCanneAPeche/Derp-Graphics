#pragma once

#include <stdint.h>
#include <functional>

#define DG_CONFIG_SCENES_TYPE uint16_t

namespace dg
{
  namespace config
  {
    enum class Scenes : DG_CONFIG_SCENES_TYPE 
    {
      MAIN = 0
    };
    
  }
}

