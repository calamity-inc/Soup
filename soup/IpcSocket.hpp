#pragma once

#include <string>

#include <Windows.h>

namespace soup
{
	struct IpcSocket
	{
		HANDLE h = INVALID_HANDLE_VALUE;

		~IpcSocket()
		{
			close();
		}

		bool bind(std::string name); // server
		bool connect(std::string name); // client
		bool accept() const noexcept; // server
		[[nodiscard]] std::string read() const;
		void write(const std::string& msg) const noexcept;
		void disconnect() const noexcept; // server
		void close() noexcept;
	};
}
