#pragma once

struct Thread
{
	void* stack;
	int status;
};

extern Thread scheduler;

extern Thread* CreateThread(void* func, void* arg);
extern void __fastcall yield(Thread* t);
extern void __fastcall restore(Thread* cur, Thread* next);
extern Thread* GetNextThread();