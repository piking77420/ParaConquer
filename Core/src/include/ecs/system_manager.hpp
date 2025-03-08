#pragma once

#include <unordered_map>

#include "ecs/ecs_system.h"
#include "reflection/reflector.hpp"

BEGIN_PCCORE

class SystemManager
{
public:

	template <SystemDerived T>
	std::shared_ptr<T> RegisterSystem()
	{
		constexpr TypeId systemTypeId = Reflector::GetTypeKey<T>();

		auto sPtr = std::make_shared<T>();
		m_Systems.insert(std::make_pair(systemTypeId, sPtr));

		return sPtr;
	}

	template <SystemDerived T>
	std::shared_ptr<T> GetSystem()
	{
		constexpr TypeId systemTypeId = Reflector::GetTypeKey<T>();
		auto it = m_Systems.find(systemTypeId);

		 if (it != m_Systems.end())
		 {
			 return *it;
		 }

		 return nullptr;
	}


	PC_FORCE_INLINE void EntityDestroyed(EntityId entityId, Signature entityIdSignature)
	{
		for (auto& it : m_Systems)
		{
			it.second->OnEntityDestroy(entityId, entityIdSignature);
		}
	}

	PC_FORCE_INLINE void EntitySignatureChanged(EntityId entityId, Signature entityIdSignature)
	{
		for (auto& it : m_Systems)
		{
			auto const& sys = it.second;

			sys->OnEntitySignatureChange(entityId, entityIdSignature);
		}
	}		


private:
	std::unordered_map<TypeId, std::shared_ptr<EcsSystem>> m_Systems;
};

REFLECT(SystemManager)

END_PCCORE