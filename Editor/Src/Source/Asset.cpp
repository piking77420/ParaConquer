#include "Asset.hpp"

#include "Log.hpp"

using namespace PC_EDITOR_CORE;



void Asset::UpdateIfPathChanged(const std::filesystem::path& _path)
{
	if (m_Path != _path)
		UpdateFromPath(_path);
}

Asset::Asset(const std::filesystem::path& _path, const PC_CORE::Guid& _guid) : m_Guid(_guid)
{
	if (!std::filesystem::exists(_path) || !_path.has_filename() || !_path.has_extension())
	{
		PC_LOG("This Path is not valid {}", _path.generic_string());
		return;
	}

	UpdateFromPath(_path);
}

Asset::Asset()
{
	if (m_FullName.empty())
		UpdateNameAndFormat();
}

void Asset::UpdateFromPath(const std::filesystem::path& _path)
{
	m_Path = _path;

	if (m_Path.has_filename())
	{
		m_FullName = m_Path.filename().generic_string();
		UpdateNameAndFormat();
	}
}

void Asset::UpdateNameAndFormat()
{
	const size_t beginFormat = m_FullName.find_first_of('.');
	if (beginFormat == std::string::npos)
		return;

	m_Name = std::string_view(m_FullName.data(), m_FullName.data() + beginFormat);
	m_Format = std::string_view(m_FullName.data() + beginFormat, m_FullName.data() + m_FullName.size() - (m_FullName.data() + beginFormat));
}
