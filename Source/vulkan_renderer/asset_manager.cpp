#include "vulkan_renderer/asset_manager.hpp"
#include <plog/Log.h>

namespace dg
{

  AssetManager::AssetManager(const std::unordered_map<uint64_t, std::string>&
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

  void AssetManager::loadTexture(uint64_t id)
  {
    if (textureMap.contains(id))
    {
      PLOG_INFO << "Texture already loaded (id=" << id << ") : " << loadMap[id];
      return;
    }

    textureMap.emplace(id, std::make_unique<Texture>(m_toolBox, loadMap[id],
          vk::ImageUsageFlagBits::eSampled));
  }

  void AssetManager::unloadTexture(uint64_t id)
  {
    textureMap.erase(id);
  }

  Texture& AssetManager::getTexture(uint64_t id)
  {
    return *textureMap.at(id);
  }

  std::weak_ptr<Texture> AssetManager::getTexturePointer(uint64_t id)
  {
    // return textureMap.at(id);
  }

  // NOTE : switching from a map to a vector to store textures might optimize this
  std::vector<vk::DescriptorImageInfo> AssetManager::textureInfos() const
  {
    uint64_t maxId = 1;
    for (const auto& [id, _] : textureMap)
      if (maxId < id) maxId = id;

    std::vector<vk::DescriptorImageInfo> infos(maxId);

    for (const auto& [id, texture] : textureMap)
      infos[id] = vk::DescriptorImageInfo({}, texture->imageView,
          vk::ImageLayout::eShaderReadOnlyOptimal);

    return infos;
  }

}
