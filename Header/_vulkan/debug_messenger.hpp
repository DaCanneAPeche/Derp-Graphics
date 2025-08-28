#pragma once

#include "vulkan/vulkan.hpp"

#include <iostream>

#include <plog/Log.h>

namespace dg
{

  vk::DebugUtilsMessengerCreateInfoEXT getDebugMessengerCreateInfo();

  vk::DebugUtilsMessengerEXT createDebugMessenger(vk::Instance& instance,
      vk::DispatchLoaderDynamic& dispatchLoader);

}
