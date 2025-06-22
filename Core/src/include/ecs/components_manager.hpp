#pragma once

#include <unordered_map>


#include "component_array.hpp"
#include "component.h"

BEGIN_PCCORE

using ComponentArrayMap = std::unordered_map<TypeId, ComponentArray>;


class ComponentManager
{
public:
	PC_CORE_API ComponentManager();

	PC_CORE_API ~ComponentManager() = default;

	template <ComponentDerived T>
	PC_FORCE_INLINE ComponentTypeBit GetComponentTypeBit() const
	{
		constexpr TypeId componentTypeId = Reflector::GetTypeKey<T>();
		
		assert(m_ComponentTypeToComponentBitFlag.find(componentTypeId) != m_ComponentTypeToComponentBitFlag.end(), "There is no component has this type");

		return m_ComponentTypeToComponentBitFlag.find(componentTypeId)->second;
	}

	PC_FORCE_INLINE ComponentTypeBit GetComponentTypeBit(TypeId id) const
	{
		assert(m_ComponentTypeToComponentBitFlag.find(id) != m_ComponentTypeToComponentBitFlag.end(), "There is no component has this type");

		return m_ComponentTypeToComponentBitFlag.find(id)->second;
	}

	template <ComponentDerived T>
	PC_FORCE_INLINE void AddComponent(EntityId entityId)
	{
		AddComponent(entityId, Reflector::GetTypeKey<T>());
	}
	
	PC_FORCE_INLINE void AddComponent(EntityId entityId,  TypeId componentType)
	{
		assert(m_ComponentTypeToComponentBitFlag.find(componentType) != m_ComponentTypeToComponentBitFlag.end(), "There is no component has this type");

		m_ComponentMapArray[componentType].AddEntityData(entityId);
	}

	PC_FORCE_INLINE void RemoveComponent(EntityId _entityId, TypeId componentType)
	{
		auto it = m_ComponentMapArray.find(componentType);
		if (it == m_ComponentMapArray.end())
		{
			PC_LOGERROR("Failed to remove component {} for entity id {}", componentType, _entityId);
			return;
		}

		it->second.RemoveEntityData(_entityId);
	}

	template <ComponentDerived T>
	PC_FORCE_INLINE bool HasComponent(EntityId _entityId)
	{
		assert(m_ComponentTypeToComponentBitFlag.find(Reflector::GetTypeKey<T>()) != m_ComponentTypeToComponentBitFlag.end(), "There is no component has this type");

		return m_ComponentMapArray[Reflector::GetTypeKey<T>()].HasComponent(_entityId);
	}

	PC_CORE_API PC_FORCE_INLINE void DestroyEntity(EntityId _entity, Signature _entitySignature)
	{
		for (uint32_t i = 0; i < m_ComponentTypeCount; i++)
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

		return reinterpret_cast<const T&>(m_ComponentMapArray.at(componentTypId).GetData(entityId));
	}

	
	uint32_t GetComponentCount() const  { return m_ComponentTypeCount; }

private:
	std::unordered_map<TypeId, ComponentTypeBit> m_ComponentTypeToComponentBitFlag;

	std::unordered_map<ComponentTypeBit, TypeId > m_ComponentBitFlagToComponentType;

	ComponentArrayMap m_ComponentMapArray;

	uint32_t m_ComponentTypeCount = 0;
	
	REFLECT(ComponentManager)
	REFLECT_MEMBER(ComponentManager, m_ComponentTypeToComponentBitFlag)
	REFLECT_MEMBER(ComponentManager, m_ComponentBitFlagToComponentType)
	REFLECT_MEMBER(ComponentManager, m_ComponentMapArray)
};



END_PCCORE