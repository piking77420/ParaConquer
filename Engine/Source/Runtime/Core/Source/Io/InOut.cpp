#include "Io/inOut.h"


#include <fstream>
#include <utility>

#include "Log.hpp"

bool PC_CORE::InOut::ReadFile(const std::filesystem::path& _path, std::vector<char>* _data)
{
	if (_path.empty())
	{
		PC_LOGERROR("Path is empty");
		return false;
	}

	const std::filesystem::path parent = _path.parent_path();
	if (!std::filesystem::exists(parent))
	{
		PC_LOGERROR("ParentPath Path doesn't exist : {}", parent.generic_string());
	}

	const std::string sPath = _path.generic_string();
	std::ifstream file(sPath, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		PC_LOGERROR("Failed to open file: {}", sPath);
		return false;
	}

	size_t fileSize = static_cast<size_t>(file.tellg());

	std::vector<char> buffer(fileSize);
	file.seekg(0);

	if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize))
	{
		PC_LOGERROR("Failed to read file: {}", sPath);
		return false;
	}

	file.close();

	*_data = buffer;
	return true;
}

PC_CORE_API bool PC_CORE::InOut::WriteFile(const std::filesystem::path& _path, const void* _data, size_t _size, bool _createDirectories)
{

	if (_path.empty())
	{
		PC_LOGERROR("Path is empty");
		return false;
	}
	const auto s = _path.generic_string();
	const std::filesystem::path parent = _path.parent_path();

	if (_createDirectories)
	{
		std::filesystem::create_directories(parent);
	}
	else
	{
		if (!std::filesystem::exists(parent))
		{
			
			PC_LOGERROR("Parent Path doesn't exist {}", s);
			return false;
		}
	}

	std::fstream f(s, std::ios::binary | std::ios::out | std::ios::trunc);

	if (!f.is_open())
	{
		PC_LOGERROR("File is not open {}", s);
		return false;
	}
	f.write(reinterpret_cast<const char*>(_data), _size);
	f.close();

	return true;
}

