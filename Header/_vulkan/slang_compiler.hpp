#pragma once

#include <string>
#include "vulkan_renderer/shader_module.hpp"

#include "slang.h"
#include "slang-com-ptr.h"
#include "slang-com-helper.h"

namespace dg
{

  class SlangCompiler
  {
    public:

      SlangCompiler(const std::string& filePath);

      ShaderModule get(const std::string& entryPoint);

    private:
      Slang::ComPtr<slang::IModule> m_module;
      Slang::ComPtr<slang::ISession> m_session;
  };

}
