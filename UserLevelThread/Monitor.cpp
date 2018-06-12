#include "Monitor.h"
#include "Thread.h"

extern "C" void __stdcall force_yield(Thread* t);

Monitor::Monitor(Scheduler* scheduler, DWORD thread_id) :
	scheduler_(scheduler), thread_(OpenThread(THREAD_GET_CONTEXT | THREAD_SET_CONTEXT, FALSE, thread_id))
{
}


Monitor::~Monitor()
{
	enable_ = false;
	WaitForSingleObject(thread_, INFINITE);
	CloseHandle(thread_);
}

void Monitor::Start()
{
	enable_ = true;
	union
	{
		LPTHREAD_START_ROUTINE r;
		decltype(&Monitor::MonitorThread) p;
	} u;
	u.p = &Monitor::MonitorThread;
	CloseHandle(CreateThread(nullptr, 0, u.r, this, 0, nullptr));
}

void __stdcall Monitor::MonitorThread()
{
	std::uint32_t count = 0;
	while (enable_)
	{
		Sleep(100);
		if (scheduler_->status == ThreadStatus::READY && scheduler_->switch_count_ == count)
		{
			SuspendThread(thread_);
			if (scheduler_->status == ThreadStatus::READY && scheduler_->switch_count_ == count)
			{
				auto current = scheduler_->next;
				CONTEXT context = { CONTEXT_CONTROL };
				GetThreadContext(thread_, &context);
				auto esp = (std::uint32_t*)context.Esp;
				*--esp = (std::uint32_t)current;
				*--esp = context.Eip;
				context.Esp = (DWORD)esp;
				context.Eip = (DWORD)&force_yield;
				context.ContextFlags = CONTEXT_CONTROL;
				SetThreadContext(thread_, &context);
			}
			ResumeThread(thread_);
		}
		count = scheduler_->switch_count_;
	}
}
