#pragma once
#include <queue>
#include <functional>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <future>

#include "PerfRegion.hpp"

namespace PC_CORE::Thread
{
	class ThreadPool
	{
	public:

		ThreadPool(const ThreadPool&) = delete;
		ThreadPool(ThreadPool&&) = delete;

		ThreadPool& operator=(ThreadPool&&) = delete;
		ThreadPool& operator=(const ThreadPool&) = delete;

		explicit ThreadPool(std::size_t nr_threads = std::thread::hardware_concurrency());

		~ThreadPool();

		template<typename F, typename... Args>
		[[nodiscard]] auto Enqueue(F&& f, Args&&... args)
			-> std::future<std::invoke_result_t<F, Args...>>
		{
			PERF_REGION_SCOPED;
			PERF_REGION_COLOR(PerfRegion::Core);

			using ReturnType = std::invoke_result_t<F, Args...>;

			auto task = std::make_shared<std::packaged_task<ReturnType()>>(
				[func = std::forward<F>(f),
				... params = std::forward<Args>(args)]() mutable {
					if constexpr (std::is_void_v<ReturnType>)
					{
						std::invoke(func, std::move(params)...);
					}
					else
					{
						return std::invoke(func, std::move(params)...);
					}
				}
			);

			std::future<ReturnType> future = task->get_future();
			{
				std::scoped_lock lock(m_Mutex);
				m_Queue.emplace([task]() {
					(*task)();
					});
			}
			m_Cv.notify_one();
			return future;
		}

	private:
		std::vector<std::jthread> m_Workers;

		std::mutex m_Mutex;

		std::condition_variable m_Cv;

		std::queue<std::function<void()>> m_Queue;

		bool m_Stop = false;

		void Worker();
	};
}