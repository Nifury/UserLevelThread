// UserLevelThread.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include <stdio.h>
#include "Thread.h"

struct Test
{
	Thread* t;

	void __stdcall TestFunc()
	{
		for (;;)
		{
			printf("test\n");
			yield(t, ThreadStatus::READY);
		}
	}
};

struct Test2
{
	Thread* t;

	void __stdcall TestFunc2()
	{
		for (;;)
		{
			printf("test2\n");
			yield(t, ThreadStatus::READY);
		}
	}
};

int main()
{
	Scheduler scheduler;

	// runze added
// 	int tickets = 10;
// 	int strides = 2;
// 	Test test;
// 	test.t = scheduler.CreateThread(Scheduler::Convert(&Test::TestFunc), &test, tickets, strides);
	// runze added
	Test test;
	Test2 test2;
	test.t = scheduler.CreateThread(Scheduler::Convert(&Test::TestFunc), &test);
	test2.t = scheduler.CreateThread(Scheduler::Convert(&Test2::TestFunc2), &test2);

	scheduler.Loop();
    return 0;
}

