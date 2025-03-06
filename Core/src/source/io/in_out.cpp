#include "io/in_out.h"

#include <iostream>

#include <fstream>
#include <utility>

void PC_CORE::InOut::PrintOut(const std::string& _string)
{
    std::cout << _string;
}

void PC_CORE::InOut::PrintOut(const std::wstring& _string)
{
    std::wcout << _string;
}

void PC_CORE::InOut::PrintOut(std::string&& _string)
{
    std::cout << _string;
}

std::vector<char> PC_CORE::InOut::ReadFile(const std::string& _filename)
{
	// Open file in binary mode at the end of the file to get the file size easily
	std::ifstream file(_filename, std::ios::ate | std::ios::binary);

	// Check if the file was opened successfully
	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file: " + _filename);
	}

	// Get the size of the file
	size_t fileSize = static_cast<size_t>(file.tellg());


	// Create a buffer of the appropriate size + 1 for '\0'
	std::vector<char> buffer(fileSize);

	// Move to the beginning of the file
	file.seekg(0);


	// Read the file data into the buffer
	if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize))
	{
		throw std::runtime_error("Failed to read file: " + _filename);
	}


	// Close the file
	file.close();
	return buffer;
}

