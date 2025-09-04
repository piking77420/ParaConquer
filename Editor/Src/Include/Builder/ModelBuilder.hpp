#pragma once

#include "EditorHeader.hpp"
#include "Resources/StaticMesh.hpp"

BEGIN_EDITOR_PCCORE

class ModelBuilder	
{
public:

	PC_CORE::StaticMesh CreateStaticMesh(const std::filesystem::path& _pathToFile);

	ModelBuilder() = default;

	~ModelBuilder() = default;
private:

};


END_EDITOR_PCCORE