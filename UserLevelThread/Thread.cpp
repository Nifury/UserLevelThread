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

// runze added
Thread* Scheduler::GetNextThreadStride()
{
	Thread next_thread;

	for (Thread& t : thread_list_)
	{
		if (t.status != ThreadStatus::READY)
		{
			continue;
		}
		if (t.status == ThreadStatus::READY || t.pass<next_thread.pass)
		{
			next_thread = t;
			next_thread.pass += next_thread.stride;
			return &next_thread;
		}
	}
	return nullptr;
}

// get random number
int rand2(void)//BR
{
	static int z1 = 12345, z2 = 12345, z3 = 12345, z4 = 12345;
	int b;
	b  = ((z1 << 6) ^ z1) >> 13;
	z1 = ((z1 & 4294967294U) << 18) ^ b;
	b  = ((z2 << 2) ^ z2) >> 27;
	z2 = ((z2 & 4294967288U) << 2) ^ b;
	b  = ((z3 << 13) ^ z3) >> 21;
	z3 = ((z3 & 4294967280U) << 7) ^ b;
	b  = ((z4 << 3) ^ z4) >> 12;
	z4 = ((z4 & 4294967168U) << 13) ^ b;
	return (z1 ^ z2 ^ z3 ^ z4);
}

Thread * Scheduler::GetNextThreadLottery()
{
	Thread next_thread;

	int total_tickets = 0;
	int golden_number;
	int count = 0;
	for (Thread& t : thread_list_)
	{
		total_tickets += t.ticket;
	}
	if (total_tickets != 0)
	{
		golden_number = rand2();
		if(golden_number < 0) {
			golden_number = golden_number * (-1);
		}
		golden_number = (golden_number % total_tickets) + 1;
	}
	for (Thread& t : thread_list_)
	{
		if (t.status != ThreadStatus::READY)
		{
			continue;
		}
		if (t.status == ThreadStatus::READY || ((count+t.ticket) < golden_number)) {
			count += t.ticket;
			continue;
		}
		next_thread = t;
		return &next_thread;
	}
	return nullptr;
}
// runze added

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

	// runze added
	if (!(ticket>0)) {
		ticket = 1;
	}
	if (!(stride>0)) {
		stride = 1;
	}
	new_thread->stride = stride;
	new_thread->ticket = ticket;
	new_thread->pass = new_thread->stride;
	// runze added
	return new_thread;
}
