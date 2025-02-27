#include "ecs/components_manager.hpp"

using namespace PC_CORE;

ComponentManager::ComponentManager()
{
	std::vector<const ReflectedType*> types = Reflector::GetAllTypesFrom<Component>();
	m_ComponentMapArray.reserve(types.size());
	m_ComponentTypeToComponentBitFlag.reserve(types.size());

	for (auto& it : types)
	{
		const ComponentArrayCreateInfo componentArrayInfo =
		{
			.componentSize = static_cast<uint32_t>(it->size),
			.constructor = it->metaData.createFunc,
			.destructor = it->metaData.deleteFunc,
		};

		m_ComponentMapArray.insert({ it->typeId, ComponentArray(ComponentArrayCreateInfo(componentArrayInfo)) });
		m_ComponentTypeToComponentBitFlag.insert({ it->typeId, componentTypeToBitSet });
		m_ComponentBitFlagToComponentType.insert({ componentTypeToBitSet,it->typeId });

		componentTypeToBitSet++;
	}
}
