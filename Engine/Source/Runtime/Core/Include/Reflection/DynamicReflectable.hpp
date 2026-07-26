#pragma once

#include "Reflection/Reflector.hpp"

BEGIN_PCCORE
    class DynamicReflectable
    {
    public:
        DEFAULT_COPY_MOVE_OPERATIONS(DynamicReflectable)

        PC_CORE_API virtual void QueryType() = 0;

        const ReflectedType& GetType() const
        {
#ifdef _DEBUG
            if (m_TypeId == NullTypeId)
            {
                PC_LOGERROR(
                    "Missing m_Type did you forget to call DYNAMIC_REFLECT_INIT or implement IMP_DYNAMIC_REFLECT")
            }
#endif

            return ReflectorInstance().GetType(m_TypeId);
        }

        const TypeId GetTypeKey() const
        {
            return m_TypeId;
        }

        DynamicReflectable() = default;

        virtual ~DynamicReflectable() = default;

    protected:
        uint32_t m_TypeId = NullTypeId;

        REFLECT(DynamicReflectable);
        REFLECT_MEMBER(DynamicReflectable, m_TypeId);
    };

END_PCCORE

#define IMP_DYNAMIC_REFLECT() \
void QueryType() override \
{\
	m_TypeId = PC_CORE::ReflectorInstance().GetTypeFromRTTI(typeid(*this).hash_code()).typeId;\
}
#define DYNAMIC_REFLECT_INIT \
QueryType();