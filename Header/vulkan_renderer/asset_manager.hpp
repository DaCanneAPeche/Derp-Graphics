#pragma once

#include <map>
#include <stdint.h>
#include <memory>

#include "_vulkan/vulkan_tool_box.hpp"
#include "vulkan_renderer/texture.hpp"

namespace dg
{

  class AssetManager
  {
    public:
      
      AssetManager(const std::unordered_map<uint64_t, std::string>& _loadMap,
          VulkanToolBox& vulkanToolBox);
      
      std::map<uint64_t, std::unique_ptr<Texture>> textureMap;
      std::unordered_map<uint64_t, std::string> loadMap;

      void loadTexture(uint64_t id);
      void unloadTexture(uint64_t id);

      Texture& getTexture(uint64_t id);
      std::weak_ptr<Texture> getTexturePointer(uint64_t id);
      std::vector<vk::DescriptorImageInfo> textureInfos() const;

      void loadAll();
      void unloadAll();

    private:
      
      VulkanToolBox& m_toolBox;

  };

}
