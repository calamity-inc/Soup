#pragma once

#include "Scheduler.hpp"

namespace soup
{
	class Server : public Scheduler
	{
	public:
		using on_accept_t = void(*)(Socket&, uint16_t port, Server&);

		on_accept_t on_accept = nullptr;

		bool bind(uint16_t port) noexcept;
	protected:
		static void setDataAvailableHandler6(Socket& s);
		static void setDataAvailableHandler4(Socket& s);
		void processAccept(Socket&& sock, uint16_t port);
	};
}
