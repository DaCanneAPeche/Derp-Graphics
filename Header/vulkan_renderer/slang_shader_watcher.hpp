#pragma once

#include <string>
#include <vector>

#include "core/file_watcher.hpp"

namespace dg
{
  class SlangShaderWatcher
  {
    public:

      void watchFile(std::string_view filePath);
      bool wereShadersModified();

    private:

      std::vector<FileWatcher> fileWatchers;
  };
}
