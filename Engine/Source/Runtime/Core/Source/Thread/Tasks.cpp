#include "Thread/Tasks.hpp"

#include "App.hpp"

#if WITH_EDITOR
#include "../Editor/Include/Editor.hpp"
#endif

// the variant to visit
using value_t = std::variant<PC_CORE::Thread::ThreadPool*, PC_CORE::Thread::TaskNode::Thread>;

// helper type for the visitor #4
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;


namespace PC_CORE::Thread
{
	TaskNode::TaskNode(Thread _Thread, std::function<void()>&& _Func, std::vector<TaskHandle> _Prerequire)
		: m_Job(std::move(_Func))
	{
		m_ThreadDesciption.emplace<Thread>(_Thread);
		for (auto& taskP : _Prerequire)
			taskP->m_Dependents.push_back(this);
		m_RemainsDepencies.store((int)_Prerequire.size(), std::memory_order_relaxed);
	}

	TaskNode::TaskNode(ThreadPool& _ThreadPool, std::function<void()>&& _Func, std::vector<TaskHandle> _Prerequire)
		: m_Job(std::move(_Func))
	{
		m_ThreadDesciption.emplace<ThreadPool*>(&_ThreadPool);
		for(auto& taskP : _Prerequire)
		{
			taskP->m_Dependents.push_back(this);

		}
		m_RemainsDepencies.store((int)_Prerequire.size(), std::memory_order_relaxed);
	}


	TaskHandle TaskScheduler::NewTask(ThreadPool& _ThreadPool, std::function<void()>&& _Func, std::vector<TaskHandle> _Prerequire)
	{
		PERF_REGION_SCOPED
		PERF_REGION_COLOR(PerfRegion::Core);

		std::scoped_lock _(m_Lock);
		std::unique_ptr Node = std::make_unique<TaskNode>(_ThreadPool, std::forward<std::function<void()>&&>(_Func), _Prerequire);
		auto ptr = Node.get();

		m_Nodes.insert(std::move(Node));
		return ptr;
	}

	TaskHandle TaskScheduler::NewTask(TaskNode::Thread _Thread, std::function<void()>&& _Func, std::vector<TaskHandle> _Prerequire)
	{
		PERF_REGION_SCOPED
		PERF_REGION_COLOR(PerfRegion::Core);


		std::scoped_lock _(m_Lock);
		std::unique_ptr Node = std::make_unique<TaskNode>(_Thread, std::forward<std::function<void()>&&>(_Func), _Prerequire);
		auto ptr = Node.get();

		m_Nodes.insert(std::move(Node));
		return ptr;
	}

	void TaskScheduler::Lauch(TaskHandle _Node)
	{
		assert(_Node);
		if (!_Node)
			return;

		Schedule(_Node);
	}

	void TaskScheduler::Decrement(TaskHandle _Node)
	{
		PERF_REGION_SCOPED
		PERF_REGION_COLOR(PerfRegion::Core);

		for (TaskHandle dep : _Node->m_Dependents)
		{
			if (dep->m_RemainsDepencies.fetch_sub(1, std::memory_order_acq_rel) == 1)
			{
				Schedule(dep);
			}
		}
	}


	void TaskScheduler::Schedule(TaskHandle _Node)
	{
		PERF_REGION_SCOPED
		PERF_REGION_COLOR(PerfRegion::Core);

		std::scoped_lock _(m_Lock);

		std::visit(overloaded{
			[this, node = _Node](ThreadPool* ThreadPool)
			{
				node->m_Future = ThreadPool->Enqueue([this, node2 = node]() {
					node2->m_Job();
					Decrement(node2);
					std::scoped_lock _(m_Lock);
					m_Nodes.erase(node2);
					});
			},
			[this, node = _Node](TaskNode::Thread _Thread)
			{
				switch (_Thread)
				{
				case TaskNode::Thread::MainThread:
					node->m_Future = App::Instance->Enqueue([this, node2 = node]()
						{
							node2->m_Job();
							Decrement(node2);
							std::scoped_lock _(m_Lock);
							m_Nodes.erase(node2);
						});
					break;
				case TaskNode::Thread::Workers:
					node->m_Future = App::Instance->Enqueue([this, node2 = node]()
						{
							node2->m_Job();
							std::scoped_lock _(m_Lock);
							m_Nodes.erase(node2);
						});
					break;
					default:
				break;
				}
			} 
			}
			,
			_Node->m_ThreadDesciption);
	}

}