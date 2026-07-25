#ifndef ASYNC_TASK_HEADER
#define ASYNC_TASK_HEADER

#include <set>
#include <variant>
#include <future>

#include "CoreHeader.hpp"
#include "ThreadPool.hpp"

namespace PC_CORE::Thread
{
	class TaskNode;
	class TaskScheduler;
}
using TaskHandle = PC_CORE::Thread::TaskNode*;


namespace PC_CORE::Thread
{
	class PC_CORE_API TaskNode
	{
	public:
		enum class Thread
		{
			MainThread,
			Workers,
		};
		TaskNode(Thread _Thread, std::function<void()>&& _Func, std::vector<TaskNode*> _Prerequire);

		TaskNode(Thread _Thread, const std::function<void()>& _Func, std::vector<TaskNode*> _Prerequire);

		TaskNode(ThreadPool& _ThreadPool, std::function<void()>&& _Func, std::vector<TaskNode*> _Prerequire);

		TaskNode(ThreadPool& _ThreadPool, const std::function<void()>& _Func, std::vector<TaskNode*> _Prerequire);

		TaskNode() = default;

		~TaskNode() = default;

		const std::future<void>& GetFuture() const
		{
			return m_Future;
		}

	private:
		std::variant<ThreadPool*, Thread> m_ThreadDesciption;

		std::function<void()> m_Job;

		std::atomic<int> m_RemainsDepencies;

		std::vector<TaskNode*> m_Dependents{};

		std::future<void> m_Future;

		friend TaskScheduler;
	};

	using TaskHandle = PC_CORE::Thread::TaskNode*;

	class PC_CORE_API TaskScheduler
	{
	public:

		TaskHandle NewTask(ThreadPool& _ThreadPool, std::function<void()>&& _Func, std::vector<TaskHandle> _Prerequire = {});

		TaskHandle NewTask(TaskNode::Thread Thread, std::function<void()>&& _Func, std::vector<TaskHandle> _Prerequire = {});

		TaskHandle NewTask(TaskNode::Thread Thread, const std::function<void()>& _Func, std::vector<TaskHandle> _Prerequire = {});

		void Lauch(TaskHandle _Node);

	private:
		struct TaskNodePtrLess
		{
			using is_transparent = void;

			bool operator()(const std::unique_ptr<TaskNode>& a,
				const std::unique_ptr<TaskNode>& b) const
			{
				return a.get() < b.get();
			}

			bool operator()(const std::unique_ptr<TaskNode>& a,
				const TaskNode* b) const
			{
				return a.get() < b;
			}

			bool operator()(const TaskNode* a,
				const std::unique_ptr<TaskNode>& b) const
			{
				return a < b.get();
			}
		};


		std::set<std::unique_ptr<TaskNode>, TaskNodePtrLess> m_Nodes;

		std::mutex m_Lock;

		void Schedule(TaskNode* _Node);

		void Decrement(TaskHandle _Node);
	};

}

using TaskThread = PC_CORE::Thread::TaskNode::Thread;

#endif //ASYNC_TASK_HEADER