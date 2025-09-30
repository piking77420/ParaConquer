#pragma once

#include "CoreHeader.hpp"
#include "Resource.hpp"
#include "Rendering/GpuResource.hpp"

BEGIN_PCCORE



class Texture : public Resource, public IGpuResource
{
public:
	
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
	Channel m_TextureChannel = Channel::DEFAULT;

	REFLECT(Texture, Resource)
	REFLECT_MEMBER(Texture, m_TextureChannel)

};



END_PCCORE