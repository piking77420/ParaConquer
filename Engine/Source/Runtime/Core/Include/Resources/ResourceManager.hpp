#pragma once

#include <functional>
#include <map>
#include <memory>
#include "ObjectPtr.hpp"

#include "CoreHeader.hpp"
#include "Singleton.hpp"
#include "Guid.hpp"
#include "Log.hpp"
#include "Resource.hpp"

BEGIN_PCCORE
    class PC_CORE_API ResourceManager
    {
    public:
        ResourceManager();

        ~ResourceManager();

        static void Destroy();

        [[nodiscard]] static bool Add(const ObjectPtr<Resource>& _object);

        template <class ResourceDerived, typename... Arg>
        static ObjectPtr<ResourceDerived> Create(Arg&& ... _args);

        template <class ResourceDerived>
        static ObjectPtr<ResourceDerived> Get(const std::string& _name);

        template <class ResourceDerived>
        static ObjectPtr<ResourceDerived> Get(const Guid& _guid);

        template <class ResourceDerived>
        static bool TryGetAs(const Guid& _guid, ObjectPtr<ResourceDerived>* _outPtr);

        static const std::string& GetName(const Guid& _guid);

        static bool Exist(const std::string& _name);

        static bool Exist(const std::string& _name, WeakObjectPtr<Resource>* outPtr);

        static bool Exist(const Guid& _guid);

        template <class ResourceDerived>
        static ObjectPtr<ResourceDerived> Get();

        template <class ResourceDerived>
        static bool Delete(const std::string& _name);

        template <class ResourceDerived>
        static void ForEach(const std::function<void(ResourceDerived*)>& _lamba);

        static void ForEach(TypeId typeID, const std::function<void(std::shared_ptr<Resource>)>& _lamba);

        static size_t GetResourceCount();

    private:
        static inline ResourceManager* m_ResourceManager = nullptr;

        static ResourceManager& Instance();

        // TODO USE FLAT MAP
        std::unordered_map<Guid, ObjectPtr<Resource>> m_ResourcesMap;

        std::unordered_map<std::string, Guid> m_NameToGuid;

        std::recursive_mutex m_lock;

        REFLECT(ResourceManager);
        REFLECT(PC_CORE::ObjectPtr<PC_CORE::Resource>);
    };


    template <class ResourceDerived, typename... Arg>
    ObjectPtr<ResourceDerived> ResourceManager::Create(Arg&&... _args)
    {
        ObjectPtr<ResourceDerived> newR = std::make_shared<ResourceDerived>(std::forward<Arg>(_args)...);

        if (!Add(std::static_pointer_cast<Resource>(newR)))
            return nullptr;

        return newR;
    }

 

    template <class ResourceDerived>
    ObjectPtr<ResourceDerived> ResourceManager::Get(const std::string& _name)
    {
        auto& instance = Instance();
        std::scoped_lock _(instance.m_lock);

        auto it = instance.m_NameToGuid.find(_name);
        if (it != instance.m_NameToGuid.end())
        {
            auto r = instance.m_ResourcesMap.at(it->second);
            assert(r->GetGuid() == it->second);
            assert(r->Name == _name);
            assert(std::dynamic_pointer_cast<ResourceDerived>(r) != nullptr);

            return std::reinterpret_pointer_cast<ResourceDerived>(r);
        }

        PC_LOGERROR("There is no resource with this name " + _name);

        return nullptr;
    }

    template <class ResourceDerived>
    ObjectPtr<ResourceDerived> ResourceManager::Get(const Guid& _guid)
    {
        auto& instance = Instance();
        std::scoped_lock _(instance.m_lock);

        assert(instance.m_ResourcesMap.contains(_guid));

        return instance.m_ResourcesMap.at(_guid);
    }

    template <class ResourceDerived>
    bool ResourceManager::TryGetAs(const Guid& _guid, ObjectPtr<ResourceDerived>* _outPtr)
    {
        auto& instance = Instance();
        std::scoped_lock _(instance.m_lock);

        auto it = instance.m_ResourcesMap.find(_guid);
        if (it == instance.m_ResourcesMap.end())
        {
            PC_LOGERROR("Cant find resource")
            return false;
        }

        if (it->second->GetType().typeId != Reflector::GetTypeKey<ResourceDerived>())
            return false;

        *_outPtr = std::reinterpret_pointer_cast<ResourceDerived>(it->second);

        return true;
    }


    template <class ResourceDerived>
    ObjectPtr<ResourceDerived> ResourceManager::Get()
    {
        auto& instance = Instance();
        std::scoped_lock _(instance.m_lock);

        for (auto it = instance.m_ResourcesMap.begin(); it != instance.m_ResourcesMap.end(); ++it)
        {
            return std::dynamic_pointer_cast<ResourceDerived>(it->second);
        }
        PC_LOGERROR("There is no resource as this type");

        return nullptr;
    }


    template <class ResourceDerived>
    bool ResourceManager::Delete(const std::string& _name)
    {
        auto& instance = Instance();
        std::scoped_lock _(instance.m_lock);

        auto itGuid = instance.m_NameToGuid.find(_name);
        if (itGuid == instance.m_NameToGuid.end())
        {
            PC_LOGERROR("There is no m_NameToGuid map {}", _name);
            return false;
        }

        auto it = instance.m_ResourcesMap.find(itGuid->second);

        if (it == instance.m_ResourcesMap.end())
        {
            PC_LOGERROR("There is no m_ResourcesMap map {}", _name);
            return false;
        }
        assert(it->second->Name == _name);

        PC_LOG("Erase resource name as {}", it->second->Name);
        it->second.reset();
        instance.m_ResourcesMap.erase(it);

        return true;
    }

    template <class ResourceDerived>
    void ResourceManager::ForEach(const std::function<void(ResourceDerived*)>& _lamba)
    {
        const TypeId typeId = Reflector::GetTypeKey<ResourceDerived>();

        auto& instance = Instance();
        std::scoped_lock _(instance.m_lock);
        for (auto it = instance.m_NameToGuid.begin(); it != instance.m_NameToGuid.end(); ++it)
        {
            const ResourceDerived* interface = reinterpret_cast<ResourceDerived*>(it->second);

            if (typeId != interface->GetType().typeId)
                continue;

            _lamba(reinterpret_cast<ResourceDerived*>(it->second));
        }
    }


END_PCCORE
