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
      
      AssetManager(const std::unordered_map<uint32_t, std::string>& _loadMap,
          VulkanToolBox& vulkanToolBox);
      
      std::map<uint32_t, std::unique_ptr<Texture>> textureMap;
      std::unordered_map<uint32_t, std::string> loadMap;

      void loadTexture(uint32_t id);
      void unloadTexture(uint32_t id);

      Texture& getTexture(uint32_t id);
      std::weak_ptr<Texture> getTexturePointer(uint32_t id);
      std::vector<vk::DescriptorImageInfo> textureInfos() const;

      void loadAll();
      void unloadAll();

    private:
      
      VulkanToolBox& m_toolBox;

  };

}
