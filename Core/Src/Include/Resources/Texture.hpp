#pragma once

#include "CoreHeader.hpp"
#include "Resource.hpp"
#include "Rendering/GpuResource.hpp"

BEGIN_PCCORE
class Texture : public Resource, public IGpuResource
{
public:

	TextureType GetType() const
	{
		return m_TextureType;
	}
	
	IMP_DYNAMIC_REFLECT()

	PC_CORE_API explicit Texture()
	{
		DYNAMIC_REFLECT_INIT;
	}

	Texture(const std::string& _name) : Resource(_name)
	{

	}


	~Texture() = default;
protected:
	REFLECT(Texture, Resource)

	TextureType m_TextureType = TextureType::Count;

	Channel m_TextureChannel;
};



END_PCCORE