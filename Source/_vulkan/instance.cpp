#include "_vulkan/instance.hpp"

namespace dg
{
  vk::Instance createInstance(std::vector<const char*> requestedExtensions,
      std::vector<const char*> validationLayers, vk::ApplicationInfo& appInfo)
  {
    vk::InstanceCreateInfo createInfo(
        vk::InstanceCreateFlags(),
        &appInfo,
        validationLayers,
        requestedExtensions
        );

    auto debugCreateInfo = getDebugMessengerCreateInfo();
    if (validationLayers.size() > 0)
    {
      createInfo.pNext = &debugCreateInfo;
    }

    return vk::createInstance(createInfo);
  }
}
