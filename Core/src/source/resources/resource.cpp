#include "resources/resource.hpp"

#include <fstream>
#include <utility>

using namespace PC_CORE;

Resource::Resource(Resource&& _other) noexcept : name(std::move(_other.name)),
format(std::move(_other.format)), 
path(std::move(_other.path)),
guid(std::move(_other.guid)) 
{

}

Resource::Resource() : guid(Guid::New())
{

}

Resource::Resource(const fs::path& _path)
{
	SetPath(_path);
	
	format = _path.extension().generic_string(); 
	const fs::path& fileName = _path.filename();
	nameWithFormat = fileName.generic_string(); 
	name = fileName.stem().generic_string();    

	guid = Guid::New();
}

void Resource::SetPath(const fs::path& _path)
{
	path = _path;
}

const std::string& Resource::GetName() const
{
	return name;
}

void Resource::WriteFile(const fs::path& folder)
{
	if (is_regular_file(folder))
	{
		PC_LOGERROR("Trying to write to a regular file");
	}
}

std::vector<char> Resource::ReadFile(const std::string& _filename)
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

