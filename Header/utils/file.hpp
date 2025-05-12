#pragma once

// std
#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>

namespace dg
{

	namespace file
	{

    template <typename T>
		std::vector<T> read(const std::string& filePath)
    {
			std::ifstream file {filePath, std::ios::ate | std::ios::binary};

			if (!file.is_open()) {
					throw std::runtime_error("Failed to open file : " + filePath);
			}

			size_t fileSize = static_cast<size_t>(file.tellg());
			std::vector<T> buffer(fileSize / sizeof(T));

			file.seekg(0);
			file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

			file.close();

			return buffer;
    }
    
    template <>
    inline std::vector<char> read<char>(const std::string& filePath)
    {
			std::ifstream file {filePath, std::ios::ate | std::ios::binary};

			if (!file.is_open()) {
					throw std::runtime_error("Failed to open file : " + filePath);
			}

			size_t fileSize = static_cast<size_t>(file.tellg());
			std::vector<char> buffer(fileSize);

			file.seekg(0);
			file.read(buffer.data(), fileSize);

			file.close();

			return buffer;
    }
		
	} /* file */ 
	
} /* dg */ 
