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
  class AssetPack;

  struct Asset
  {
    std::string path;
    std::weak_ptr<AssetManager> manager; 
    AssetPack* pPack = nullptr;
  };

  struct TextureAsset : public Asset
  {
    bool loadedOnGpu = false;
    uint32_t index;
    float imageRatio = 1;

    ~TextureAsset();
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
      // void removeGapsOnTheGPU() {}; // TODO : Implement

    private:
      VulkanToolBox& m_toolBox;
      std::vector<std::unique_ptr<Texture>> m_textures;
  };

  class AssetPack
  {
    public:
      static std::shared_ptr<AssetManager> s_assetManager;

      static void initAssetManager(VulkanToolBox& vulkanToolBox)
      {
        if (s_assetManager == nullptr)
          s_assetManager = std::make_shared<AssetManager>(vulkanToolBox);
      }

    private:

    protected:
      TextureAsset texture(const std::string& assetName);
  };

}
