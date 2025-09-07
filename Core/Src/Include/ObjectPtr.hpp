#pragma once

#include <memory>

#include "Reflection/Reflector.hpp"

BEGIN_PCCORE

template<class T>
concept DynamicReflectableDerived = std::is_base_of_v<DynamicReflectable, T>;

template<DynamicReflectableDerived T>
class ObjectPtr final : public std::shared_ptr<T>
{
public:
    using Base = std::shared_ptr<T>;



    ObjectPtr() = default;

    ~ObjectPtr() = default;

    ObjectPtr(T* ptr) : Base(ptr) {}

    ObjectPtr(std::shared_ptr<T>&& ptr) noexcept : Base(std::move(ptr)) {}

    ObjectPtr(const std::shared_ptr<T>& ptr) : Base(ptr) {}

    template<DynamicReflectableDerived U>
        requires std::is_base_of_v<T, U>
    ObjectPtr(const std::shared_ptr<U>& ptr) : Base(ptr) {}

    template<DynamicReflectableDerived U>
        requires std::is_base_of_v<T, U>
    ObjectPtr(std::shared_ptr<U>&& ptr) : Base(std::move(ptr)) {}

    template<DynamicReflectableDerived U>
        requires std::is_base_of_v<T, U>
    ObjectPtr& operator=(const std::shared_ptr<U>& ptr) { Base::operator=(ptr); return *this; }

    template<DynamicReflectableDerived U>
        requires std::is_base_of_v<T, U>
    ObjectPtr& operator=(std::shared_ptr<U>&& ptr) { Base::operator=(std::move(ptr)); return *this; }

};

template<DynamicReflectableDerived T>
class WeakObjectPtr final : public std::weak_ptr<T>
{
public:
    using Base = std::weak_ptr<T>;


    ObjectPtr<T> Lock() const
    {
        return ObjectPtr<T>(Base::lock());
    }

    bool IsValid() const { return !Base::expired(); }

    size_t UseCount() const { return Base::use_count(); }

    WeakObjectPtr() = default;

    WeakObjectPtr(const ObjectPtr<T>& _objectPtr) : Base(_objectPtr) {}

    WeakObjectPtr(WeakObjectPtr&& _weak) noexcept : Base(std::move(_weak)) {}

    WeakObjectPtr(const WeakObjectPtr& _weak) : Base(_weak) {}

    template<DynamicReflectableDerived U>
        requires std::is_base_of_v<T, U>
    WeakObjectPtr(const std::weak_ptr<U>& ptr) : Base(ptr) {}

    template<DynamicReflectableDerived U>
        requires std::is_base_of_v<T, U>
    WeakObjectPtr(std::weak_ptr<U>&& ptr) : Base(std::move(ptr)) {}

    template<DynamicReflectableDerived U>
        requires std::is_base_of_v<T, U>
    WeakObjectPtr& operator=(const std::weak_ptr<U>& ptr) { Base::operator=(ptr); return *this; }

    template<DynamicReflectableDerived U>
        requires std::is_base_of_v<T, U>
    WeakObjectPtr& operator=(std::weak_ptr<U>&& ptr) { Base::operator=(std::move(ptr)); return *this; }

    WeakObjectPtr operator=(const ObjectPtr<T>& _objectPtr) 
    {
        Base::operator=(_objectPtr); 
        return *this;                
    }

    WeakObjectPtr operator=(ObjectPtr<T>&& _objectPtr) 
    {
        Base::operator=(std::move(_objectPtr));
        return *this;
    }

private:

};



END_PCCORE