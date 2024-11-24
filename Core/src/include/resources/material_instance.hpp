#pragma once

#include "texture.hpp"
#include "rendering/render_harware_interface/rhi_typedef.h"
#include "resources/Resource.hpp"


BEGIN_PCCORE

class Material;

class MaterialInstance : public Resource
{
public:
	Material* materialRef = nullptr;

	Texture* albedo = nullptr;

	PC_CORE_API MaterialInstance(std::string _materialInstanceName, Material* _material);

	PC_CORE_API ~MaterialInstance();

	PC_CORE_API DescriptorSetHandle GetDescriptorSetHandle() const;

	PC_CORE_API void BuildDescriptorSet();
};

END_PCCORE