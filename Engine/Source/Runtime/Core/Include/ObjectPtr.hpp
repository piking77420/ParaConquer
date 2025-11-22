#pragma once

#include <memory>
#include "Resources/Resource.hpp"

BEGIN_PCCORE
    template <ResourceDerived T>
    class ObjectPtr final : public std::shared_ptr<T>
    {
    public:
        using Base = std::shared_ptr<T>;
        using Obj = T;

        explicit operator bool() const noexcept
        {
            return Base::operator bool();
        }

        ObjectPtr() = default;

        ~ObjectPtr() = default;

        ObjectPtr(T* ptr) : Base(ptr)
        {
        }

        ObjectPtr(std::shared_ptr<T>&& ptr) noexcept : Base(std::move(ptr))
        {
        }

        ObjectPtr(const std::shared_ptr<T>& ptr) : Base(ptr)
        {
        }

        template <ResourceDerived U>
            requires std::is_base_of_v<T, U>
        ObjectPtr(const std::shared_ptr<U>& ptr) : Base(ptr)
        {
        }

        template <ResourceDerived U>
            requires std::is_base_of_v<T, U>
        ObjectPtr(std::shared_ptr<U>&& ptr) : Base(std::move(ptr))
        {
        }

        template <ResourceDerived U>
            requires std::is_base_of_v<T, U>
        ObjectPtr& operator=(const std::shared_ptr<U>& ptr)
        {
            Base::operator=(ptr);
            return *this;
        }

        template <ResourceDerived U>
            requires std::is_base_of_v<T, U>
        ObjectPtr& operator=(std::shared_ptr<U>&& ptr)
        {
            Base::operator=(std::move(ptr));
            return *this;
        }
    };

    template <ResourceDerived T>
    class WeakObjectPtr final : public std::weak_ptr<T>
    {
    public:
        using Base = std::weak_ptr<T>;
        using Obj = T;

        ObjectPtr<T> Lock() const
        {
            return ObjectPtr<T>(Base::lock());
        }

        bool IsValid() const { return !Base::expired(); }

        size_t UseCount() const { return Base::use_count(); }

        explicit operator bool() const noexcept
        {
            return Base::operator bool();
        }

        WeakObjectPtr() = default;

        WeakObjectPtr(const ObjectPtr<T>& _objectPtr) : Base(_objectPtr)
        {
        }

        WeakObjectPtr(WeakObjectPtr&& _weak) noexcept : Base(std::move(_weak))
        {
        }

        WeakObjectPtr(const WeakObjectPtr& _weak) : Base(_weak)
        {
        }


        template <ResourceDerived U>
            requires std::is_base_of_v<T, U>
        WeakObjectPtr(const std::weak_ptr<U>& ptr) : Base(ptr)
        {
        }

        template <ResourceDerived U>
            requires std::is_base_of_v<T, U>
        WeakObjectPtr(std::weak_ptr<U>&& ptr) : Base(std::move(ptr))
        {
        }

        template <ResourceDerived U>
            requires std::is_base_of_v<T, U>
        WeakObjectPtr& operator=(const std::weak_ptr<U>& ptr)
        {
            Base::operator=(ptr);
            return *this;
        }

        template <ResourceDerived U>
            requires std::is_base_of_v<T, U>
        WeakObjectPtr& operator=(std::weak_ptr<U>&& ptr)
        {
            Base::operator=(std::move(ptr));
            return *this;
        }

        WeakObjectPtr& operator=(const ObjectPtr<T>& _objectPtr)
        {
            Base::operator=(_objectPtr);
            return *this;
        }

        WeakObjectPtr& operator=(ObjectPtr<T>&& _objectPtr)
        {
            Base::operator=(std::move(_objectPtr));
            return *this;
        }

        WeakObjectPtr& operator=(WeakObjectPtr<T>&& _wobjectPtr)
        {
            Base::operator=(std::move(_wobjectPtr));
            return *this;
        }

        WeakObjectPtr& operator=(const WeakObjectPtr<T>& _wobjectPtr)
        {
            Base::operator=(_wobjectPtr);
            return *this;
        }
    };

END_PCCORE
