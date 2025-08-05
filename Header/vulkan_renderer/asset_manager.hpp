#pragma once

#include <map>
#include <stdint.h>
#include <memory>

#include "_vulkan/vulkan_tool_box.hpp"
#include "vulkan_renderer/texture.hpp"
#include "_vulkan/descriptor_set_manager.hpp"

namespace dg
{

  /*class AssetManager
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

  };*/

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
