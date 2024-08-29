#pragma once

// std
#include <stack>
#include <functional>

// vulkan
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.hpp>

#include "dg_device.hpp"

namespace dg
{
	inline vma::Allocator gAllocator;
} /* dg */ 
