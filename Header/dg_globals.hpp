#pragma once

// std
#include <stack>
#include <functional>

// vulkan
#include <vulkan/vulkan.hpp>

#include "dg_device.hpp"

namespace dg
{
	namespace g
	{
		inline std::stack<std::function<void(vk::Instance&)>> instanceCleaning;
		inline std::stack<std::function<void(dg::Device&)>> deviceCleaning;
	} /* g */ 
} /* dg */ 
