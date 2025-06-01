#include "ecs/components_manager.hpp"

using namespace PC_CORE;

ComponentManager::ComponentManager()
{
	std::vector<const ReflectedType*> types = Reflector::GetAllTypesFrom<Component>();
	m_ComponentMapArray.reserve(types.size());
	m_ComponentTypeToComponentBitFlag.reserve(types.size());

	ComponentTypeBit typeBit = 0;
	for (auto& it : types)
	{

		m_ComponentMapArray.insert({ it->typeId, ComponentArray(it->typeId) });
		m_ComponentTypeToComponentBitFlag.insert({ it->typeId, typeBit });
		m_ComponentBitFlagToComponentType.insert({ typeBit,it->typeId });

		typeBit++;
		m_ComponentTypeCount++;
	}
}
