#ifndef CORE_SCOPE_GUARD
#define CORE_SCOPE_GUARD

#include <utility>

namespace PC_CORE
{
	template <typename F>
	class ScopeGuard
	{
	public:
		explicit ScopeGuard(F f)
			: m_Func(std::move(f))
		{

		}

		~ScopeGuard()
		{
			m_Func();
		}

	private:
		F m_Func;
	};

	template <typename F>
	ScopeGuard<F> MakeScopeGuard(F f) {
		return ScopeGuard<F>(f);
	}

} // namespace namespace 

#endif // CORE_SCOPE_GUARD