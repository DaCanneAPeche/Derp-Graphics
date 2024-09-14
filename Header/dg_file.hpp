#pragma once

// std
#include <vector>
#include <string>

namespace dg
{

	namespace file
	{

		[[nodiscard]] std::vector<char> read(const std::string& filePath);
		
	} /* file */ 
	
} /* dg */ 
