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
		for (int i = 0; i < 5; ++i)
		{
			printf("test\n");
			Sleep(1000);
			yield(t, ThreadStatus::READY);
		}
	}
};

int main()
{
	Scheduler scheduler;

	Test test;
	test.t = scheduler.CreateThread(Scheduler::Convert(&Test::TestFunc), &test);

	scheduler.Loop();
    return 0;
}

