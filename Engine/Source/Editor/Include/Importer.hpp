#pragma once

#include "EditorHeader.hpp"
#include <string>
#include <unordered_map>

#include "Resources/Resource.hpp"
#include "ObjectPtr.hpp"

BEGIN_PCCORE
    class Serializer;
END_PCCORE

BEGIN_EDITOR_PCCORE
    class Importer
    {
    public:
        [[nodiscard]] bool Import(const std::filesystem::path& _path, PC_CORE::Serializer* _serializer,
                                  PC_CORE::TypeId* _outId, PC_CORE::ObjectPtr<PC_CORE::Resource>* _outResource) const;

    private:
        [[nodiscard]] bool ImportTexture(const std::filesystem::path& _path, PC_CORE::Serializer* _serializer,
                                         PC_CORE::TypeId* _outId,
                                         PC_CORE::ObjectPtr<PC_CORE::Resource>* _outResource) const;


        [[nodiscard]] bool ImportMesh(const std::filesystem::path& _path, PC_CORE::Serializer* _serializer,
                                      PC_CORE::TypeId* _outId,
                                      PC_CORE::ObjectPtr<PC_CORE::Resource>* _outResource) const;

        [[nodiscard]] bool ImportStaticMesh(const std::filesystem::path& _path, PC_CORE::Serializer* _serializer,
                                            PC_CORE::TypeId* _outId,
                                            PC_CORE::ObjectPtr<PC_CORE::Resource>* _outResource) const;
    };

END_EDITOR_PCCORE
