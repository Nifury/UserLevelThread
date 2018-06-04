#include "Thread.h"
#include <windows.h>

extern "C" void thread_func();

void Scheduler::Loop()
{
	while (true)
	{
		auto next = GetNextThread();
		if (next)
		{
			this->next = next;
			yield(this, ThreadStatus::READY);
			if (next->status == ThreadStatus::ZOMBIE)
			{
				free_list_.emplace_back(next);
			}
		}
		else
		{
			//TODO: wait for IOCP
			printf("No thread to schedule\n");
			return;
		}
	}
}

Thread* Scheduler::GetNextThread()
{
	for (Thread& t : thread_list_)
	{
		if (t.status == ThreadStatus::READY)
			return &t;
	}
	return nullptr;
}

Scheduler::Scheduler() :
	monitor_(this, GetCurrentThreadId())
{
	switch_count_ = 0;
	monitor_.Start();
}

Thread* Scheduler::CreateThread(Func func, void* arg)
{
	constexpr int DEFAULT_STACK_SIZE = 4 << 20;
	Thread* new_thread;
	if (free_list_.empty())
	{
		thread_list_.emplace_back();
		new_thread = &thread_list_.back();
		new_thread->stack_data = VirtualAlloc(0, DEFAULT_STACK_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		new_thread->next = this;
	}
	else
	{
		new_thread = free_list_.back();
		free_list_.pop_back();
	}

	void** p = (void**)(int(new_thread->stack_data) + DEFAULT_STACK_SIZE);

	*--p = new_thread;
	*--p = arg;
	*--p = func;
	*--p = &thread_func;
	new_thread->stack = (void*)p;
	new_thread->status = ThreadStatus::READY;
	new_thread->switch_count_ = 0;

	return new_thread;
}
