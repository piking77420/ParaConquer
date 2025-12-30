#pragma once
#include <type_traits>

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
	}
}