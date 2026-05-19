#pragma once

#include <string>
#include <filesystem>

namespace dg
{
  struct FileWatcher
  {
    FileWatcher(std::string_view filePath) : path(filePath)
    {
      lastFileSaveTime = std::filesystem::last_write_time(path);
    }

    bool wasFileWritten()
    {
      auto fileSaveTime = std::filesystem::last_write_time(path);

      if (lastFileSaveTime == fileSaveTime) return false;

      lastFileSaveTime = fileSaveTime;
      return true;
    }

    std::filesystem::path path;
    std::filesystem::file_time_type lastFileSaveTime;

  };
}
