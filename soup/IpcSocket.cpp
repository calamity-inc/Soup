#include "base.hpp"
#if SOUP_WINDOWS

#include "IpcSocket.hpp"

#pragma comment(lib, "Advapi32.lib")

namespace soup
{
	bool IpcSocket::bind(std::string name)
	{
		// Allow unprivileged processes to connect to servers hosted by privileged processes. (https://stackoverflow.com/a/57755632)
		SECURITY_DESCRIPTOR sd;
		InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
		SetSecurityDescriptorDacl(&sd, TRUE, 0, FALSE);
		SetSecurityDescriptorControl(&sd, SE_DACL_PROTECTED, SE_DACL_PROTECTED);
		SECURITY_ATTRIBUTES sa = { sizeof(sa), &sd, FALSE };

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
			&sa
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
