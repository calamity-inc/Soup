#include "SelfDeletingThread.hpp"

#include "log.hpp"

namespace soup
{
	SelfDeletingThread::SelfDeletingThread(void(*f)(Capture&&), Capture&& cap) noexcept
		: Thread(run, this), f(f), cap(std::move(cap))
	{
	}

	void SelfDeletingThread::run(Capture&& cap)
	{
		auto t = cap.get<SelfDeletingThread*>();
		try
		{
			t->f(std::move(t->cap));
		}
		catch (const std::exception& e)
		{
			std::string msg = "Exception in SelfDeletingThread: ";
			msg.append(e.what());
			logWriteLine(std::move(msg));
		}
		delete t;
	}
}
