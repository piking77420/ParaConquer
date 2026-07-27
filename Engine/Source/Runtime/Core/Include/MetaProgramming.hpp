#pragma once
#include <type_traits>
#include <utility>

namespace PC_CORE
{
	namespace MetaProgramming
	{
		template <typename>
		struct is_const_member_function : std::false_type {};

		template <typename C, typename Ret, typename... Args>
		struct is_const_member_function<Ret(C::*)(Args...) const>
			: std::true_type {
		};

		template <typename C, typename Ret, typename... Args>
		struct is_const_member_function<Ret(C::*)(Args...)>
			: std::false_type {
		};

		template <bool Cst, typename Ret, typename... Args>
		struct TrampolineMemberFunc
		{
			template <typename T, Ret(T::* Method)(Args...)>
			static Ret Call(void* obj, Args... args) {
				return (static_cast<T*>(obj)->*Method)(std::forward<Args>(args)...);
			}
		};

		// None const 
		template <typename Ret>
		struct TrampolineMemberFunc<false, Ret, void>
		{
			template <typename T, Ret(T::* Method)()>
			static Ret Call(void* obj) {
				return (static_cast<T*>(obj)->*Method)();
			}
		};

		template <typename... Args>
		struct TrampolineMemberFunc<false, void, Args...>
		{
			template <typename T, void(T::* Method)(Args...)>
			static void Call(void* obj, Args... args) {
				return (static_cast<T*>(obj)->*Method)(std::forward<Args>(args)...);
			}
		};

		// Const
		template <typename Ret>
		struct TrampolineMemberFunc<true, Ret, void>
		{
			template <typename T, Ret(T::* Method)() const>
			static Ret Call(const void* obj) {
				return (static_cast<const T*>(obj)->*Method)();
			}
		};

		template <typename... Args>
		struct TrampolineMemberFunc<true, void, Args...>
		{
			template <typename T, void(T::* Method)(Args...) const>
			static void Call(const void* obj, Args... args) {
				return (static_cast<const T*>(obj)->*Method)(std::forward<Args>(args)...);
			}
		};
	}
}