#include "base.hpp"
#if SOUP_WINDOWS

#include "IpcSocket.hpp"

namespace soup
{
	bool IpcSocket::bind(std::string name)
	{
		// Currently, IpcSocket created by privileged process can not be connected to by unprivileged processes.
		// To rectify this, we should specify the appropriate security attributes: https://stackoverflow.com/a/57755632

		name.insert(0, "\\\\.\\pipe\\");
		close();
		h = CreateNamedPipeA(
			name.c_str(),
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
			1,
			4096,
			4096,
			0x7fffffff,
			nullptr
		);
		return h != INVALID_HANDLE_VALUE;
	}

	bool IpcSocket::connect(std::string name)
	{
		name.insert(0, "\\\\.\\pipe\\");
		close();
		h = CreateFileA(
			name.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,
			nullptr,
			OPEN_EXISTING,
			0,
			nullptr
		);
		return h != INVALID_HANDLE_VALUE;
	}

	bool IpcSocket::accept() const noexcept
	{
		return ConnectNamedPipe(h, nullptr) != FALSE;
	}

	std::string IpcSocket::read() const
	{
		std::string ret{};

		char buffer[4096];
		DWORD bytesRead;
		if (ReadFile(h, buffer, sizeof(buffer), &bytesRead, nullptr) != FALSE)
		{
			ret = std::string((const char*)&buffer[0], bytesRead);
		}

		return ret;
	}

	void IpcSocket::write(const std::string& msg) const noexcept
	{
		WriteFile(h, msg.data(), (DWORD)msg.size(), nullptr, nullptr);
	}

	void IpcSocket::disconnect() const noexcept
	{
		DisconnectNamedPipe(h);
	}

	void IpcSocket::close() noexcept
	{
		if (h != INVALID_HANDLE_VALUE)
		{
			CloseHandle(h);
			h = INVALID_HANDLE_VALUE;
		}
	}
}

#endif
