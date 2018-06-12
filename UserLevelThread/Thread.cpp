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
	for (int i = 0, len = thread_list_.size(); i < len; ++i)
	{
		auto ret = thread_list_[last_pos_++];
		last_pos_ = last_pos_ % len;
		if (ret->status == ThreadStatus::READY)
			return ret;
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
		new_thread = new Thread;
		new_thread->stack_data = VirtualAlloc(0, DEFAULT_STACK_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		new_thread->next = this;
		thread_list_.push_back(new_thread);
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
