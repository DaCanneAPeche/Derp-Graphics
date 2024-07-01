#include "dg_logger.hpp"

// std
#include <span>

namespace dg
{

	Logger::Logger() {};

	Logger& Logger::get() {
		static Logger instance;
		return instance;
	}
	
	void Logger::iMsg(std::string message)
	{
		std::cout << message;
	}

	void Logger::msg(const std::string& message)
	{
		if (!get().isEnabled) return;

		get().iMsg(message);
	}

	void Logger::msgLn(const std::string& message)
	{
		if (!get().isEnabled) return;

		get().iMsg(message + '\n');
	}

	void Logger::msgVector(const std::vector<std::string>& messageList,
			const std::string& separator, const std::string& end)
	{
		if (!get().isEnabled) return;
		Logger& instance = get();

		for (size_t i = 0 ; i < messageList.size() ; i++)
		{
			instance.iMsg(messageList[i]);
			if (i + 1 < messageList.size()) instance.iMsg(separator);
		}
		
		instance.iMsg(end);
	}

	void Logger::msgCStringArray(const char** messageList, int length,
			const std::string& separator, const std::string& end)
	{
		if (!get().isEnabled) return;
		Logger& instance = get();

		for (size_t i = 0 ; i < length ; i++)
		{
			instance.iMsg(messageList[i]);
			if (i + 1 < length) instance.iMsg(separator);
		}

		instance.iMsg(end);
	}
	
	void Logger::msgExtensions(std::vector<vk::ExtensionProperties>& extensions,
			const std::string& separator, const std::string& end)
	{
		if (!get().isEnabled) return;
		Logger& instance = get();

		for (size_t i = 0 ; i < extensions.size() ; i++)
		{
			instance.iMsg(extensions[i].extensionName);
			if (i + 1 < extensions.size()) instance.iMsg(separator);
		}

		instance.iMsg(end);
	}
	
	void Logger::logPhysicalDevice(const vk::PhysicalDevice& device)
	{
		if (!get().isEnabled) return;
		vk::PhysicalDeviceProperties properties = device.getProperties();
		get().iMsg("Device : " + static_cast<std::string>(properties.deviceName) + '\n');
		get().iMsg("Type : ");
		switch (properties.deviceType) {

			case (vk::PhysicalDeviceType::eCpu):
				std::cout << "CPU";
				break;

			case (vk::PhysicalDeviceType::eDiscreteGpu):
				std::cout << "Discrete GPU";
				break;

			case (vk::PhysicalDeviceType::eIntegratedGpu):
				std::cout << "Integrated GPU";
				break;

			case (vk::PhysicalDeviceType::eVirtualGpu):
				std::cout << "Virtual GPU";
				break;

			default:
				std::cout << "Other";
		}
		get().iMsg("\n");
	}
		
}
