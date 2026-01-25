#include "Thread/ThreadPool.hpp"

#include <string>

#include "PerfRegion.hpp"
#include "Thread/ThreadUtils.hpp"

namespace PC_CORE::Thread
{
	ThreadPool::ThreadPool(const char* ThreadPoolName, std::size_t NbrofThread)
	{
		for (size_t i = 0; i < NbrofThread; i++)
		{
			m_Workers.emplace_back(std::jthread([this, i, ThreadPoolName] {
				Utils::SetThreadName((std::string(ThreadPoolName) + std::string(" Worker") + std::to_string(i)).c_str());
				Worker();
				}));
		}
	}

	ThreadPool::~ThreadPool()
	{
		{
			std::scoped_lock lock(m_Mutex);
			m_Stop = true;
		}

		m_Cv.notify_all();
		m_Workers.clear();
	}
	

	void ThreadPool::Worker()
	{
		for (;;) 
		{
			std::function<void()> task;
			{
				std::unique_lock<std::mutex> _(m_Mutex);
				m_Cv.wait(_, [this]() {
					return m_Stop || !m_Queue.empty();
					});

				if (m_Stop && m_Queue.empty())
					break;
				if (m_Queue.empty())
				{
					std::this_thread::sleep_for(std::chrono::microseconds(1));
					continue;
				}

				task = m_Queue.front();
				m_Queue.pop();
				
			}
			task();
		}
	}
		
}