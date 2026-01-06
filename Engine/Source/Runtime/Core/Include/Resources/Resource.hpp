#pragma once

#include <array>
#include <set>


#include "Log.hpp"
#include "CoreHeader.hpp"


#include "Guid.hpp"
#include "Serialize/Iseriazable.h"
#include "ResourceFormat.hpp"
#include "Reflection/DynamicReflectable.hpp"
#include "Serialize/Serializer.h"


BEGIN_PCCORE
    class ResourceManager;

    class Resource : public DynamicReflectable
    {
    public:

        DEFAULT_COPY_MOVE_OPERATIONS(Resource);

        PC_CORE_API Resource(const std::string& _name);

        PC_CORE_API Resource(std::string&& _name);

        PC_CORE_API Resource() = default;

        PC_CORE_API ~Resource() override = default;

        static PC_CORE_API void LinkDependencies(Resource* _resourceParent, Resource* _resourceChild);


        PC_CORE_API void QueryType() override = 0;

        // Reload base on modificated parent ?
        PC_CORE_API virtual void OnParentReload(const Guid& _parentGuid)
        {
            PC_LOG("OnParentReload {}", Name)
        };

        PC_CORE_API virtual void Reload()
        {
            PC_LOG("Reload {}", Name)
        }

        PC_CORE_API virtual void AfterSerialize(Serializer* serializer) const
        {
        };

        PC_CORE_API virtual void AfterDeSerialize(Serializer* serializer)
        {
        };

        // Reload 
        PC_CORE_API void BroadCastReload();

        PC_CORE_API const Guid& GetGuid() const
        {
            return m_Guid;
        }

        std::string Name; // TODO PROTECT IT 

    protected:
        Guid m_Guid = Guid::New();

        const std::vector<Guid>& GetParentResource() const
        {
            return m_ParentsResource;
        }

        const std::vector<Guid>& GetChildResource() const
        {
            return m_ChildsResource;
        }

        PC_CORE_API static std::string GetFullExtension(const std::string& _path);

    private:
        std::vector<Guid> m_ParentsResource;

        std::vector<Guid> m_ChildsResource;

        REFLECT(Resource, DynamicReflectable)
        REFLECT_MEMBER(Resource, Name)
        REFLECT_MEMBER(Resource, m_Guid)
        REFLECT_MEMBER(Resource, m_ParentsResource)
        REFLECT_MEMBER(Resource, m_ChildsResource)
    };

END_PCCORE
