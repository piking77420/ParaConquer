#pragma once

#include <string>
#include <string_view>

#include "CoreHeader.hpp"
#include "RhiTypedef.h"

BEGIN_PCCORE

class Rhi;

class RhiObject
{
public:

	PC_CORE_API virtual ~RhiObject();

	PC_CORE_API explicit RhiObject(Rhi& rhi, const std::string& _name);

    PC_CORE_API explicit RhiObject(Rhi& rhi, std::string&& _name);

	PC_CORE_API virtual const void* GetFrameNativeHandle(size_t _frameIndex) const = 0;

	PC_CORE_API virtual void* GetFrameNativeHandle(size_t _frameIndex) = 0;

	PC_CORE_API virtual bool Build() = 0;

protected:
	Rhi& m_Rhi;

protected:
	std::string m_Name;

};

template<typename T>
class RhiObjectT : public RhiObject
{
public:
	~RhiObjectT() override = default;

	DEFAULT_COPY_MOVE_OPERATIONS(RhiObjectT);

	explicit RhiObjectT(Rhi& rhi, const std::string& _name)
		: RhiObject(rhi, _name)
	{

	}

	explicit RhiObjectT(Rhi& rhi, std::string&& _name)
		: RhiObject(rhi, std::move(_name))
	{

	}

	virtual const void* GetFrameNativeHandle(size_t _frameIndex) const = 0;

	virtual void* GetFrameNativeHandle(size_t _frameIndex) = 0;

	virtual bool Build() = 0;


	auto& SetName(const char* _Name)
	{	
		m_Name = std::string(_Name);
		return *this;
	}

	auto& SetName(std::string_view _Name)
	{
		m_Name.append(_Name);
		return *this;
	}

	auto& SetName(const std::string& _Name)
	{
		m_Name = _Name;
		return *this;
	}

	auto& SetName(std::string&& _Name)
	{
		m_Name = std::move(_Name);
		return *this;
	}

	std::string_view GetName() const noexcept
	{
		return std::string_view(m_Name);
	}

private:

};


END_PCCORE

