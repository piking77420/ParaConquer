#pragma once

#include "core_header.hpp"
#include "resource.hpp"
#include "rendering/gpu_resource.hpp"

BEGIN_PCCORE
class Texture : public Resource, public IGpuResource
{
public:

	TextureType GetType() const
	{
		return m_TextureType;
	}
	
	IMP_DYNAMIC_REFLECT()

		PC_CORE_API    explicit Texture()
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

	int m_TextureChannel = -1;
};



END_PCCORE