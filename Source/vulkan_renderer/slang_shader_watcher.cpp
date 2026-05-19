#include "vulkan_renderer/slang_shader_watcher.hpp"

namespace dg
{
  void SlangShaderWatcher::watchFile(std::string_view filePath)
  {
    fileWatchers.emplace_back(filePath);
  }

  bool SlangShaderWatcher::wereShadersModified()
  {
    for (auto& fileWatcher : fileWatchers)
    {
      if (fileWatcher.wasFileWritten()) return true;
    }

    return false;
  }
}
