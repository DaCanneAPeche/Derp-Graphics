#include "_vulkan/debug_messenger.hpp"

namespace dg
{
  static VKAPI_ATTR VkBool32 VKAPI_CALL _debugCallback(
      VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
      VkDebugUtilsMessageTypeFlagsEXT messageType,
      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
      void* pUserDate
      )
  {
    if (messageSeverity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) return VK_TRUE;

    LOG_FATAL << "Validation layer : " << pCallbackData->pMessage;
    std::cerr << "Validation layer : " << pCallbackData->pMessage << std::endl << std::endl;

    return VK_FALSE;
  }

  vk::DebugUtilsMessengerCreateInfoEXT getDebugMessengerCreateInfo()
  {
    vk::DebugUtilsMessengerCreateInfoEXT messengerInfo(
        {},
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose,
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding,
        _debugCallback
        );

    return messengerInfo;
  }

  vk::DebugUtilsMessengerEXT createDebugMessenger(vk::Instance& instance,
      vk::DispatchLoaderDynamic& dispatchLoader)
  {
    vk::DebugUtilsMessengerCreateInfoEXT messengerInfo = getDebugMessengerCreateInfo();

      vk::DebugUtilsMessengerEXT debugMessenger =
        instance.createDebugUtilsMessengerEXT(
            messengerInfo,
            nullptr,
            dispatchLoader);

      return debugMessenger;
    }

}
