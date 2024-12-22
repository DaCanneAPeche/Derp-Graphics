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

    textureMap.insert(
        { id, Texture(m_toolBox, loadMap[id]) }
        );
  }

  void AssetManager::unloadTexture(uint64_t id)
  {
    textureMap.erase(id);
  }

  Texture& AssetManager::getTexture(uint64_t id)
  {
    return textureMap.at(id);
  }

  Texture* AssetManager::getTexturePointer(uint64_t id)
  {
    return &textureMap.at(id);
  }

}
