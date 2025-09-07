#pragma once

#include "EditorHeader.hpp"
#include <string>
#include <unordered_map>

#include "Resources/Resource.hpp"
#include "ObjectPtr.hpp"


BEGIN_EDITOR_PCCORE

class Importer
{
public:

	[[nodiscard]] bool Import(const std::filesystem::path& _path, PC_CORE::TypeId* _outId, PC_CORE::ObjectPtr<PC_CORE::Resource>* _outResource) const;

private:


};

END_EDITOR_PCCORE