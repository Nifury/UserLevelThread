#pragma once

#include <windows.h>

struct Scheduler;

class Monitor
{
public:
	Monitor(Scheduler* scheduler, DWORD thread_id);
	~Monitor();

	void Start();
	void End()
	{
		enable_ = false;
	}

private:
	void __stdcall MonitorThread();

	Scheduler* scheduler_;
	HANDLE thread_;
	bool enable_ = false;
};

