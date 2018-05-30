#pragma once
#include <vector>
#include <memory>

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

	virtual ~Scheduler()
	{}

	Thread* CreateThread(Func func, void* arg);
	void Loop();

protected:
	virtual Thread * GetNextThread();

	std::vector<Thread> thread_list_;

private:
	std::vector<Thread*> free_list_;
};
