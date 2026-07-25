#pragma once

#include <string>
#include <string_view>
#include <type_traits>

#include "CoreHeader.hpp"
#include "RhiTypedef.h"

namespace PC_CORE
{

class Rhi;

class RhiObject
{
public:
	PC_CORE_API explicit RhiObject(Rhi& rhi);

	PC_CORE_API virtual ~RhiObject();

	RhiObject(const RhiObject&) = delete;

	RhiObject& operator=(const RhiObject&) = delete;

	RhiObject(RhiObject&&) = default;

	PC_CORE_API virtual bool Build() = 0;

	PC_CORE_API Rhi& GetRhi();

protected:
	Rhi& m_Rhi;

protected:
	std::string m_Name;

};

template<typename T>
class RhiObjectT : public RhiObject
{
public:
	explicit RhiObjectT(Rhi& rhi)
		: RhiObject(rhi)
	{

	}

	~RhiObjectT() override = default;

	bool Build() override = 0;

	T& SetName(const char* _Name)
	{
		static_assert(std::is_base_of_v<RhiObject, T>, "T must be an RhiObject");

		m_Name = std::string(_Name);
		return reinterpret_cast<T&>(*this);
	}

	T& SetName(std::string_view _Name)
	{
		static_assert(std::is_base_of_v<RhiObject, T>, "T must be an RhiObject");

		m_Name.append(_Name);
		return reinterpret_cast<T&>(*this);
	}

	T& SetName(const std::string& _Name)
	{
		static_assert(std::is_base_of_v<RhiObject, T>, "T must be an RhiObject");

		m_Name = _Name;
		return reinterpret_cast<T&>(*this);
	}

	T& SetName(std::string&& _Name)
	{
		static_assert(std::is_base_of_v<RhiObject, T>, "T must be an RhiObject");

		m_Name = std::move(_Name);
		return reinterpret_cast<T&>(*this);
	}

	std::string_view GetName() const noexcept
	{
		return std::string_view(m_Name);
	}

private:

};

}

