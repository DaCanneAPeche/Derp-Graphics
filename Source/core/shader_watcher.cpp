#include "core/shader_watcher.hpp"

namespace dg
{
  void ShaderWatcher::watchFile(std::string_view filePath)
  {
    fileWatchers.emplace_back(filePath);
  }

  void ShaderWatcher::process()
  {
    if (p_renderer == nullptr)
      throw std::runtime_error("ShaderWatcher's pRenderer was not initizialed !");

    wereShadersModifiedOnLastFrame = false;
    for (auto& fileWatcher : fileWatchers)
    {
      if (fileWatcher.wasFileWritten()) wereShadersModifiedOnLastFrame = true;
    }

    if (wereShadersModifiedOnLastFrame)
    {
      p_renderer->pipelinesInfo.clear();
      callback();
      p_renderer->recreateSwapChain();
    }
  }
}
