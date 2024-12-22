#pragma once

#include <map>
#include <stdint.h>

#include "_vulkan/vulkan_tool_box.hpp"
#include "vulkan_renderer/texture.hpp"

namespace dg
{

  class AssetManager
  {
    public:
      
      AssetManager(const std::unordered_map<uint64_t, std::string>& _loadMap,
          VulkanToolBox& vulkanToolBox);
      
      std::map<uint64_t, Texture> textureMap;
      std::unordered_map<uint64_t, std::string> loadMap;

      void loadTexture(uint64_t id);
      void unloadTexture(uint64_t id);

      Texture& getTexture(uint64_t id);
      Texture* getTexturePointer(uint64_t id);

      void loadAll();
      void unloadAll();

    private:
      
      VulkanToolBox& m_toolBox;

  };

}
