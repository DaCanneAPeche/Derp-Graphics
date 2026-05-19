#pragma once

#include <string>
#include <vector>
#include <functional>

#include "core/file_watcher.hpp"
#include "vulkan_renderer/renderer.hpp"

namespace dg
{
  class ShaderWatcher
  {
    public:

      void watchFile(std::string_view filePath);
      void process();

      void initRenderer(Renderer& renderer)
      {
        p_renderer = &renderer;
      }

      std::function<void()> callback = {};
      bool wereShadersModifiedOnLastFrame = false;

    private:

      std::vector<FileWatcher> fileWatchers;
      Renderer* p_renderer;
  };
}
