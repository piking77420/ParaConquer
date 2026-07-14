#ifndef CORE_FREE_LIST
#define CORE_FREE_LIST

#include <vector>
#include <stack>

namespace PC_CORE
{
	template <typename T>
	class FreeList
	{
	public:
		FreeList() = default;
		~FreeList() = default;
		FreeList(FreeList&&) = default;
		FreeList(const FreeList&) = default;
		FreeList& operator=(const FreeList&) = default;

		[[nodiscard]] std::pair<std::size_t, T*> Emplace(T&& object)
		{
			std::size_t index;
			T* pointer = nullptr;

			if (m_FreeIndices.empty())
			{
				index = m_Slots.size();
				pointer = &m_Slots.emplace_back(std::move(object));
			}
			else
			{
				index = m_FreeIndices.back();
				m_FreeIndices.pop_back();

				m_Slots[index] = std::move(object);
				pointer = &m_Slots[index];
			}

			return { index, pointer };
		}

		[[nodiscard]]
		std::pair<std::size_t, T*> Emplace(const T& object) requires std::copy_constructible<T> && std::assignable_from<T&, const T&>
		{
			std::size_t index;
			T* pointer = nullptr;

			if (m_FreeIndices.empty())
			{
				index = m_Slots.size();
				pointer = &m_Slots.emplace_back(object);
			}
			else
			{
				index = m_FreeIndices.back();
				m_FreeIndices.pop_back();

				m_Slots[index] = object;
				pointer = &m_Slots[index];
			}

			return { index, pointer };
		}

		[[nodiscard]] T* Get(size_t _Index)
		{
			if (!Contains(_Index))
				return nullptr;

			return &m_Slots[_Index];
		}

		[[nodiscard]] const T* Get(size_t _Index) const
		{
			if (!Contains(_Index))
				return nullptr;

			return &m_Slots[_Index];
		}

		bool Remove(size_t _Index)
		{
			if (!Contains(_Index))
				return false;

			m_Slots[_Index] = {};
			m_FreeIndices.push_back(_Index);

			return true;
		}

		[[nodiscard]] bool Contains(size_t _Index) const noexcept
		{
			return _Index >= m_Slots.size();
		}

	private:
		std::vector<T> m_Slots;
		std::vector<size_t> m_FreeIndices;
	};

} // namespace PC_CORE

#endif // CORE_FREE_LIST