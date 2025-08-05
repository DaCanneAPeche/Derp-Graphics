#include "vulkan_renderer/asset_manager.hpp"
#include <plog/Log.h>

namespace dg
{

  /*AssetManager::AssetManager(const std::unordered_map<uint32_t, std::string>&
      _loadMap, VulkanToolBox& vulkanToolBox) : m_toolBox(vulkanToolBox),
  loadMap(_loadMap) { }

  void AssetManager::loadAll()
  {
    for (const auto& [id, filePath] : loadMap)
    {
      loadTexture(id);
    }
  }
  
  void AssetManager::unloadAll()
  {
    textureMap.clear();
  }

  void AssetManager::loadTexture(uint32_t id)
  {
    if (textureMap.contains(id))
    {
      PLOG_INFO << "Texture already loaded (id=" << id << ") : " << loadMap[id];
      return;
    }

    textureMap.emplace(id, std::make_unique<Texture>(m_toolBox, loadMap[id],
          vk::ImageUsageFlagBits::eSampled));
  }

  void AssetManager::unloadTexture(uint32_t id)
  {
    textureMap.erase(id);
  }

  Texture& AssetManager::getTexture(uint32_t id)
  {
    return *textureMap.at(id);
  }

  std::weak_ptr<Texture> AssetManager::getTexturePointer(uint32_t id)
  {
    // return textureMap.at(id);
  }

  // NOTE : switching from a map to a vector to store textures might optimize this
  std::vector<vk::DescriptorImageInfo> AssetManager::textureInfos() const
  {
    uint32_t maxId = 0;
    for (const auto& [id, _] : textureMap) if (maxId < id) maxId = id;

    std::vector<vk::DescriptorImageInfo> infos(maxId + 1);

    for (const auto& [id, texture] : textureMap)
      infos[id] = vk::DescriptorImageInfo({}, texture->imageView,
          vk::ImageLayout::eShaderReadOnlyOptimal);

    return infos;
  }*/

  uint32_t TextureAsset::loadAndGetIndex()
  {
    assert(pManager != nullptr && "pManager was not assigned");
    if (!loadedOnGpu) pManager->loadTexture(*this);
    return index;
  }

  void AssetManager::loadTexture(TextureAsset& asset)
  {
    if (asset.loadedOnGpu)
    {
      LOG_INFO << "Tried to load texture " << asset.path << " but was already loaded";
      return;
    }

    bool emptySpaceFound = false;
    for (size_t i = 0 ; i < m_textures.size() ; i++)
    {
      if (m_textures[i] == nullptr)
      {
        asset.index = i;
        emptySpaceFound = true;
        break;
      }
    }

    if (emptySpaceFound) m_textures[asset.index] = std::make_unique<Texture>(m_toolBox,
        asset.path, vk::ImageUsageFlagBits::eSampled);

    else {
      m_textures.push_back(std::make_unique<Texture>(m_toolBox, asset.path,
            vk::ImageUsageFlagBits::eSampled));
      asset.index = m_textures.size() - 1;
    }

    textureDescriptorUpdates.push_back({asset.index, vk::DescriptorImageInfo({}, 
           m_textures[asset.index]->imageView, vk::ImageLayout::eShaderReadOnlyOptimal)});

    asset.loadedOnGpu = true;
    LOG_INFO << "Loaded texure " << asset.path;
  }

  void AssetManager::unloadTexture(TextureAsset& asset)
  {
    if (!asset.loadedOnGpu)
    {
      LOG_INFO << "Tried to unload texture " << asset.path << " but was not loaded";
      return;
    }

    m_textures[asset.index] = nullptr;
    LOG_INFO << "Unloaded texture " << asset.path;
  }

  TextureAsset AssetManager::texture(const std::string& assetName)
  {
    TextureAsset asset;
    asset.pManager = this;
    asset.path = assetName;
    return asset;
  }

  void AssetManager::processTextureLoadings(DescriptorWriter& descriptorWriter)
  {
    for (const auto& update : textureDescriptorUpdates)
    {
      descriptorWriter.writeToImage(update.writeInfo, update.index);
    }
  }
}
