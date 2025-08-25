#pragma once

#include <string>
#include <string_view>
#include <filesystem>

#include "EditorHeader.hpp"
#include "Guid.hpp"

BEGIN_EDITOR_PCCORE

class Asset
{
public:

	inline const std::filesystem::path& GetPath() const
	{
		return m_Path;
	}

	inline const std::string& GetFullName() const 
	{
		return m_FullName;
	}

	inline const std::string_view& GetName() const
	{
		return m_Name;
	}

	inline const std::string_view& GetFormat() const
	{
		return m_Format;
	}

	inline const PC_CORE::Guid& GetGuid() const  
	{
		return m_Guid;
	}

	operator bool() const 
	{
		return std::filesystem::exists(m_Path) && !m_FullName.empty() 
			&& !m_Name.empty() && !m_Format.empty();
	}

	void UpdateIfPathChanged(const std::filesystem::path& _path);

	Asset(const std::filesystem::path& _path, const PC_CORE::Guid& _guid);

	Asset();

	~Asset() = default;

private:
	std::filesystem::path m_Path;

	std::string m_FullName;

	std::string_view m_Name;

	std::string_view m_Format;

	PC_CORE::Guid m_Guid;

	void UpdateFromPath(const std::filesystem::path& _path);

	void UpdateNameAndFormat();

	REFLECT(Asset);
	REFLECT_MEMBER(Asset, m_Path);
	REFLECT_MEMBER(Asset, m_FullName);
	REFLECT_MEMBER(Asset, m_Guid);
};

END_EDITOR_PCCORE