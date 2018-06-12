#pragma once
#include <vector>
#include <memory>
#include "Monitor.h"

struct Scheduler;
struct Thread;
enum class ThreadStatus;

extern "C" void __fastcall yield(Thread* t, ThreadStatus status);

enum class ThreadStatus
{
	ZOMBIE, READY, RUNNING, WAITING, 
};

struct Thread
{
	void* stack;
	ThreadStatus status;
	Thread* next;

	void* stack_data;
	std::uint32_t switch_count_;
	// runze added
	int ticket;
	int stride;
	int pass;
	// runze added
};

struct Scheduler : Thread
{
public:
	using Func = void(__stdcall*)(void*);

	template<typename F>
	inline static Func Convert(F f)
	{
		union {
			F f_;
			Func ret;
		};
		f_ = f;
		return ret;
	}

	Scheduler();

	virtual ~Scheduler()
	{}

	virtual Thread* CreateThread(Func func, void* arg);
	void Loop();

protected:
	virtual Thread * GetNextThread();

	std::vector<Thread*> thread_list_;
	int last_pos_ = 0;

private:
	std::vector<Thread*> free_list_;
	Monitor monitor_;
};

struct LotteryScheduler : Scheduler
{
public:
	virtual Thread* CreateThread(Func func, void* arg) override;
    Thread* CreateThread(Func func, void* arg, int tickets);

protected:
	virtual Thread * GetNextThread() override;
};

struct StrideScheduler : Scheduler
{
public:
	virtual Thread* CreateThread(Func func, void* arg) override;
	Thread* CreateThread(Func func, void* arg, int tickets, int strides);

protected:
	virtual Thread * GetNextThread() override;
};