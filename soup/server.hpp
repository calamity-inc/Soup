#pragma once

#include "scheduler.hpp"

namespace soup
{
	struct server : public scheduler
	{
		using on_accept_t = void(*)(socket&, uint16_t port);

		on_accept_t on_accept = nullptr;

		bool bind(uint16_t port) noexcept;
	protected:
		static void setDataAvailableHandler6(socket& s);
		static void setDataAvailableHandler4(socket& s);
		void processAccept(socket&& sock, uint16_t port);
	};
}
