#include "Thread.h"

Thread* LotteryScheduler::CreateThread(Func func, void* arg, int tickets)
{
	auto thread = Scheduler::CreateThread(func, arg);
	//FIXME
	if (!(tickets > 0)) {
		tickets = 1;
	}
	thread->ticket = tickets;
	return thread;
}

Thread* LotteryScheduler::CreateThread(Func func, void* arg)
{
	return CreateThread(func, arg, 20);
}

// get random number
static int rand2(void)//BR
{
	static int z1 = 12345, z2 = 12345, z3 = 12345, z4 = 12345;
	int b;
	b = ((z1 << 6) ^ z1) >> 13;
	z1 = ((z1 & 4294967294U) << 18) ^ b;
	b = ((z2 << 2) ^ z2) >> 27;
	z2 = ((z2 & 4294967288U) << 2) ^ b;
	b = ((z3 << 13) ^ z3) >> 21;
	z3 = ((z3 & 4294967280U) << 7) ^ b;
	b = ((z4 << 3) ^ z4) >> 12;
	z4 = ((z4 & 4294967168U) << 13) ^ b;
	return (z1 ^ z2 ^ z3 ^ z4);
}

Thread * LotteryScheduler::GetNextThread()
{
	int total_tickets = 0;
	int golden_number;
	int count = 0;
	for (auto t : thread_list_)
	{
		total_tickets += t->ticket;
	}
	if (total_tickets != 0)
	{
		golden_number = rand2();
		if (golden_number < 0) {
			golden_number = golden_number * (-1);
		}
		golden_number = (golden_number % total_tickets) + 1;
	}
	for (auto t : thread_list_)
	{
		if (t->status != ThreadStatus::READY)
		{
			continue;
		}
		if (((count + t->ticket) < golden_number)) {
			count += t->ticket;
			continue;
		}
		return t;
	}
	return nullptr;
}