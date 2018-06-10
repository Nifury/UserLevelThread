#include "Thread.h"


Thread* StrideScheduler::CreateThread(Func func, void* arg)
{
	return CreateThread(func, arg, 20, 2);
}

Thread* StrideScheduler::CreateThread(Func func, void* arg, int tickets, int strides)
{
	auto thread = Scheduler::CreateThread(func, arg);
	if (!(ticket > 0)) {
		ticket = 1;
	}
	if (!(stride > 0)) {
		stride = 1;
	}
	thread->stride = strides;
	thread->ticket = tickets;
	thread->pass = thread->stride;
	return thread;
}

Thread * StrideScheduler::GetNextThread()
{
	Thread* next_thread = nullptr;
	for (Thread& t : thread_list_)
	{
		if (t.status != ThreadStatus::READY)
		{
			continue;
		}
		if (next_thread == nullptr || (t.pass < next_thread->pass))
		{
			next_thread = &t;
			next_thread->pass += next_thread->stride;
			return next_thread;
		}
	}
	return nullptr;
}
