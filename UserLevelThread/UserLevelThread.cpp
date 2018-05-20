// UserLevelThread.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include <stdio.h>
#include "Thread.h"

struct Test
{
	Thread* t;

	Test()
	{
		union {
			decltype(&Test::TestFunc) f;
			void* p;
		} u;
		u.f = &Test::TestFunc;
		t = CreateThread(u.p, this);
	}

	void __stdcall TestFunc()
	{
		while (true)
		{
			printf("test\n");
			Sleep(1000);
			yield(t);
		}
	}
};

int main()
{
	Test test;
	while (true)
	{
		printf("scheduler\n");
		restore(&scheduler, GetNextThread());
	}
    return 0;
}

