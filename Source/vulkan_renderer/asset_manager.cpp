#include "vulkan_renderer/asset_manager.hpp"
#include <plog/Log.h>

namespace dg
{

  TextureAsset::~TextureAsset()
  {
    if (!loadedOnGpu) return;

    if (auto sManager = manager.lock()) sManager->unloadTexture(*this);
  }

  uint32_t TextureAsset::loadAndGetIndex()
  {
    assert(pPack != nullptr && "pPack is not a valid pointer");
    
    if (!loadedOnGpu)
    {
      if (auto sManager = manager.lock()) sManager->loadTexture(*this);
      else
      {
        assert(pPack->s_assetManager != nullptr && "assetManager is not valid");
        manager = pPack->s_assetManager;
        return loadAndGetIndex();
      }
    }

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
    asset.imageRatio = m_textures[asset.index]->getRatio();
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

  void AssetManager::processTextureLoadings(DescriptorWriter& descriptorWriter)
  {
    for (const auto& update : textureDescriptorUpdates)
    {
      descriptorWriter.writeToImage(update.writeInfo, update.index);
    }
  }

  std::shared_ptr<AssetManager> AssetPack::s_assetManager = nullptr;

  TextureAsset AssetPack::texture(const std::string& assetName)
  {
    TextureAsset asset;
    asset.pPack = this;
    asset.path = assetName;
    return asset;
  }
}
