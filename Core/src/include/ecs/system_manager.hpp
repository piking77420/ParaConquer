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
		auto system = std::make_shared<T>();
		m_Systems.emplace_back(system);
		return system;
	}

	template <SystemDerived T>
	std::shared_ptr<T> GetSystem()
	{
		constexpr TypeId systemTypeId = Reflector::GetTypeKey<T>();

		for (auto it = m_Systems.begin(); it != m_Systems.end(); ++it)
		{
			if (it->get()->GetTypeKey() == systemTypeId)
			{
				return it;
			}
		}
		
		 return nullptr;
	}


	PC_FORCE_INLINE void EntityDestroyed(EntityId entityId, Signature entityIdSignature)
	{
		for (auto& it : m_Systems)
			it->OnEntityDestroy(entityId, entityIdSignature);
		
	}

	PC_FORCE_INLINE void EntitySignatureChanged(EntityId entityId, Signature entityIdSignature)
	{
		for (auto& it : m_Systems)
			it->OnEntitySignatureChange(entityId, entityIdSignature);
		
	}		


private:
	std::vector<std::shared_ptr<EcsSystem>> m_Systems;
};

REFLECT(SystemManager)

END_PCCORE