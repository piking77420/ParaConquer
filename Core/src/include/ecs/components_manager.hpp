#pragma once

#include <unordered_map>


#include "component_array.hpp"
#include "component.h"

BEGIN_PCCORE

class ComponentManager
{
public:
	PC_CORE_API ComponentManager();

	PC_CORE_API ~ComponentManager() = default;

	template <ComponentDerived T>
	PC_FORCE_INLINE ComponentTypeBit GetComponentType() const
	{
		constexpr TypeId componentTypeId = Reflector::GetTypeKey<T>();


		assert(m_ComponentTypeToComponentBitFlag.find(componentTypeId) != m_ComponentTypeToComponentBitFlag.end(), "There is no component has this type");

		return m_ComponentTypeToComponentBitFlag.find(componentTypeId)->second;
	}

	template <ComponentDerived T>
	PC_FORCE_INLINE void AddComponent(EntityId entityId)
	{
		assert(m_ComponentTypeToComponentBitFlag.find(Reflector::GetTypeKey<T>()) != m_ComponentTypeToComponentBitFlag.end(), "There is no component has this type");

		m_ComponentMapArray[Reflector::GetTypeKey<T>()].AddEntityData(entityId);
	}

	PC_FORCE_INLINE void RemoveComponent(EntityId _entityId, TypeId componentType)
	{
		m_ComponentMapArray[componentType].RemoveEntityData(_entityId);
	}

	template <ComponentDerived T>
	PC_FORCE_INLINE bool HasComponent(EntityId _entityId)
	{
		assert(m_ComponentTypeToComponentBitFlag.find(Reflector::GetTypeKey<T>()) != m_ComponentTypeToComponentBitFlag.end(), "There is no component has this type");

		return m_ComponentMapArray[Reflector::GetTypeKey<T>()].HasComponent(_entityId);
	}

	PC_CORE_API PC_FORCE_INLINE void DestroyEntity(EntityId _entity, Signature _entitySignature)
	{
		for (uint32_t i = 0; i < componentTypeToBitSet; i++)
		{
			if (_entitySignature.test(i))
			{
				m_ComponentMapArray[m_ComponentBitFlagToComponentType[i]].RemoveEntityData(_entity);
			}
		}

		
	}

	template <ComponentDerived T>
	PC_FORCE_INLINE T& GetComponent(EntityId entityId)
	{
		constexpr TypeId componentTypId = Reflector::GetTypeKey<T>();

		return reinterpret_cast<T&>(m_ComponentMapArray[componentTypId].GetData(entityId));
	}

	template <ComponentDerived T>
	PC_FORCE_INLINE const T& GetComponent(EntityId entityId) const 
	{
		constexpr TypeId componentTypId = Reflector::GetTypeKey<T>();

		return reinterpret_cast<const T&>(m_ComponentMapArray[componentTypId].GetData(entityId));
	}


private:
	ComponentTypeBit componentTypeToBitSet = 0;

	std::unordered_map<TypeId, ComponentTypeBit> m_ComponentTypeToComponentBitFlag;

	std::unordered_map<ComponentTypeBit, TypeId > m_ComponentBitFlagToComponentType;

	std::unordered_map<TypeId, ComponentArray> m_ComponentMapArray;

};

END_PCCORE