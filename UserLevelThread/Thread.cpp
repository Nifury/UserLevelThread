#include "Thread.h"

Thread scheduler;
Thread thread_list;

Thread* GetNextThread()
{
	return &thread_list;
}

__declspec(naked) void __fastcall restore(Thread* cur, Thread* next)
{
	__asm
	{
		MOV[ECX], ESP;
		MOV ESP, [EDX];
		RET;
	}
}

__declspec(naked) void __fastcall yield(Thread* t)
{
	__asm
	{
		PUSHAD;
		PUSHFD;
		MOV EDX, OFFSET scheduler;
		CALL restore;
		POPFD;
		POPAD;
		RETN;
	}
}

static __declspec(naked) void thread_func()
{
	__asm
	{
		POP EAX;
		CALL EAX;
		ADD ESP, 4;
		POP ECX;
		MOV[ECX + 4], 0;
		CALL yield;
	}
}

Thread* CreateThread(void* func, void* arg)
{
	char* stack = new char[4096];
	stack = stack + 4096 - 16;
	void** p = (void**)stack;
	*p++ = &thread_func;
	*p++ = func;
	*p++ = arg;
	*p++ = &thread_list;
	thread_list.stack = stack;
	return &thread_list;
}