#pragma once

#include <stack>
#include <functional>
#include <vulkan/vulkan.hpp>

namespace dg
{
	namespace g
	{
		inline std::stack<std::function<void(vk::Instance&)>> instanceCleaning;
		inline std::stack<std::function<void(vk::Device&)>> deviceCleaning;
	} /* g */ 
} /* dg */ 
