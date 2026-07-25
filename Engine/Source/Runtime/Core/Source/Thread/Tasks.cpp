#include "Thread/Tasks.hpp"

#include "App.hpp"
namespace PC_CORE::Thread
{
	// Task Nodes

	TaskNode::TaskNode(Thread _Thread, std::function<void()>&& _Func, std::vector<TaskHandle> _Prerequire)
		: m_Job(std::move(_Func))
	{
		m_ThreadDesciption.emplace<Thread>(_Thread);
		for (auto& taskP : _Prerequire)
			taskP->m_Dependents.push_back(this);
		m_RemainsDepencies.store(static_cast<int>(_Prerequire.size()), std::memory_order_relaxed);
	}

	TaskNode::TaskNode(Thread _Thread, const std::function<void()>& _Func, std::vector<TaskNode*> _Prerequire)
		: m_Job(_Func)
	{
		m_ThreadDesciption.emplace<Thread>(_Thread);
		for (auto& taskP : _Prerequire)
			taskP->m_Dependents.push_back(this);
		m_RemainsDepencies.store(static_cast<int>(_Prerequire.size()), std::memory_order_relaxed);
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

	TaskNode::TaskNode(ThreadPool& _ThreadPool, const std::function<void()>& _Func, std::vector<TaskNode*> _Prerequire)
		: m_Job(_Func)
	{
		m_ThreadDesciption.emplace<ThreadPool*>(&_ThreadPool);
		for (auto& taskP : _Prerequire)
		{
			taskP->m_Dependents.push_back(this);
		}
		m_RemainsDepencies.store((int)_Prerequire.size(), std::memory_order_relaxed);
	}

	// TaskHandle
	
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

	TaskHandle TaskScheduler::NewTask(TaskNode::Thread _Thread, const std::function<void()>& _Func, std::vector<TaskHandle> _Prerequire)
	{
		PERF_REGION_SCOPED
		PERF_REGION_COLOR(PerfRegion::Core);
		std::scoped_lock _(m_Lock);
		std::unique_ptr Node = std::make_unique<TaskNode>(_Thread, std::forward<const std::function<void()>&>(_Func), _Prerequire);
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
					auto it = m_Nodes.find(node2);
					if (it != m_Nodes.end())
						m_Nodes.erase(it);
						
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
							auto it = m_Nodes.find(node2);
							if (it != m_Nodes.end())
								m_Nodes.erase(it);
						});
					break;
				case TaskNode::Thread::Workers:
					node->m_Future = App::Instance->Enqueue([this, node2 = node]()
						{
							node2->m_Job();
							std::scoped_lock _(m_Lock);
							auto it = m_Nodes.find(node2);
							if (it != m_Nodes.end())
								m_Nodes.erase(it);
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