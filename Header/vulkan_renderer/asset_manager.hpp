#pragma once

#include <map>
#include <stdint.h>
#include <memory>

#include "_vulkan/vulkan_tool_box.hpp"
#include "vulkan_renderer/texture.hpp"
#include "_vulkan/descriptor_set_manager.hpp"

namespace dg
{

  class AssetManager;

  struct Asset
  {
    std::string path;
    AssetManager* pManager; 
  };

  struct TextureAsset : public Asset
  {
    bool loadedOnGpu = false;
    uint32_t index;
    float imageRatio = 1;

    uint32_t loadAndGetIndex();
  };

  struct TextureDescriptorUpdate
  {
    uint32_t index;
    vk::DescriptorImageInfo writeInfo;
  };

  /*struct BatchedTextureDescriptorUpdate
  {
    uint32_t index;
    std::vector<vk::DescriptorImageInfo> writeInfos;
  };*/

  class AssetManager
  {
    public:
      AssetManager(VulkanToolBox& vulkanToolBox) : m_toolBox(vulkanToolBox) {}

      std::vector<TextureDescriptorUpdate> textureDescriptorUpdates;
      void loadTexture(TextureAsset& asset);
      void unloadTexture(TextureAsset& asset);
      void processTextureLoadings(DescriptorWriter& descriptorWriter);

      void batchTextureUpdates() {}; // TODO : Implement

      // Move the textures on the VRAM to save memory, probably not very useful
      void removeGapsOnTheGPU() {}; // TODO : Implement

    private:
      VulkanToolBox& m_toolBox;
      std::vector<std::unique_ptr<Texture>> m_textures;

    protected:
      TextureAsset texture(const std::string& assetName);
  };

}
