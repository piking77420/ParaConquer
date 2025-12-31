#pragma once

#include "EditorHeader.hpp"
#include <string>
#include <unordered_map>

#include "Resources/Resource.hpp"
#include "ObjectPtr.hpp"

BEGIN_PCCORE
    class Serializer;
    class Rhi;
    class StaticMesh;
END_PCCORE

BEGIN_EDITOR_PCCORE
    class Importer
    {
    public:
        [[nodiscard]] bool Import(PC_CORE::Rhi& _Rhi, const std::filesystem::path& _path, PC_CORE::Serializer* _serializer,
                                  PC_CORE::TypeId* _outId, PC_CORE::ObjectPtr<PC_CORE::Resource>* _outResource) const;

        [[nodiscard]] bool ImportTexture(PC_CORE::Rhi& _Rhi, const std::filesystem::path& _path, PC_CORE::Serializer* _serializer,
                                         PC_CORE::TypeId* _outId,
                                         PC_CORE::ObjectPtr<PC_CORE::Resource>* _outResource) const;


        [[nodiscard]] bool ImportMesh(PC_CORE::Rhi& _Rhi, const std::filesystem::path& _path, PC_CORE::Serializer* _serializer,
                                      PC_CORE::TypeId* _outId,
                                      PC_CORE::ObjectPtr<PC_CORE::Resource>* _outResource) const;

        [[nodiscard]] bool ImportStaticMesh(PC_CORE::Rhi& _Rhi, const std::filesystem::path& _path, PC_CORE::Serializer* _serializer,
                                            PC_CORE::TypeId* _outId,
                                            PC_CORE::ObjectPtr<PC_CORE::Resource>* _outResource) const;

        [[nodiscard]] bool ImportMesh(PC_CORE::Rhi& _Rhi, const std::filesystem::path& _path, PC_CORE::StaticMesh* _StaticMesh) const;
    };

END_EDITOR_PCCORE
