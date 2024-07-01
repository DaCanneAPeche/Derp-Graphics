#pragma once

// std
#include <string>
#include <vector>
#include <iostream>

#include "vulkan/vulkan.hpp"

namespace dg
{
	
	// Singleton
	class Logger
	{
	public:
		Logger ();

		static Logger& get();
		static void msg(const std::string& message);

		static void msgLn(const std::string& message);

		static void msgVector(const std::vector<std::string>& messageList,
				const std::string& separator = ", ", const std::string& end = "\n");

		static void msgCStringArray(const char** messageList, int length,
				const std::string& separator = ", ", const std::string& end = "\n");

		static void msgExtensions(std::vector<vk::ExtensionProperties>& extensions,
				const std::string& separator = ", ", const std::string& end = "\n");

		static void logPhysicalDevice(const vk::PhysicalDevice& device);
		
		inline static bool isEnabled = false;

	private:

		void iMsg(std::string message);

	};
	
} /* dg */ 
