#pragma once

#include "base.hpp"

#if SOUP_WINDOWS
#pragma comment(lib, "Ws2_32.lib")
#include <Winsock2.h>
#include <Ws2tcpip.h>

#pragma comment(lib, "Secur32.lib")
#define SECURITY_WIN32
#include <sspi.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> // close
#include <fcntl.h>
#endif

#include "addr_socket.hpp"

namespace soup
{
	class socket
	{
	public:
#if SOUP_WINDOWS
		using fd_t = SOCKET;
#else
		using fd_t = int;
#endif
		fd_t fd;

#if SOUP_WINDOWS
		inline static size_t wsa_consumers = 0;
		static PSecurityFunctionTableA sft;

		bool encrypted = false;
		CtxtHandle ctx_h;
		CredHandle cred_h;
		SecPkgContext_StreamSizes Sizes{};
		static constexpr int MaxMsgSize = 16000;
		static constexpr int MaxExtraSize = 50;
		char writeBuffer[MaxMsgSize + MaxExtraSize]{};
		char readBuffer[(MaxMsgSize + MaxExtraSize) * 2]{};
		size_t readBufferBytes = 0;
		char plainText[MaxMsgSize * 2]{};
		char* plainTextPtr = nullptr;
		size_t plainTextBytes = 0;
		void* readPtr = nullptr;
#endif

		socket() noexcept
			: fd(-1)
		{
			on_construct();
#if SOUP_WINDOWS
			SecInvalidateHandle(&ctx_h);
			SecInvalidateHandle(&cred_h);
#endif
		}

		socket(const socket&) = delete;

		socket(socket&& b) noexcept
			: fd(b.fd)
		{
			on_construct();
			operator =(std::move(b));
		}

	protected:
		void on_construct()
		{
#if SOUP_WINDOWS
			if (wsa_consumers++ == 0)
			{
				WSADATA wsaData;
				WORD wVersionRequested = MAKEWORD(2, 2);
				WSAStartup(wVersionRequested, &wsaData);
			}
#endif
		}

	public:
		void operator =(const socket&) = delete;

		void operator =(socket&& b) noexcept
		{
			fd = b.fd;
			b.fd = -1;

#if SOUP_WINDOWS
			ctx_h.dwLower = b.ctx_h.dwLower;
			ctx_h.dwUpper = b.ctx_h.dwUpper;
			SecInvalidateHandle(&b.ctx_h);

			cred_h.dwLower = b.cred_h.dwLower;
			cred_h.dwUpper = b.cred_h.dwUpper;
			SecInvalidateHandle(&b.cred_h);
#endif
		}

		~socket() noexcept
		{
			close();
#if SOUP_WINDOWS
			if (SecIsValidHandle(&cred_h))
			{
				sft->FreeCredentialsHandle(&cred_h);
				SecInvalidateHandle(&cred_h);
			}
			if (--wsa_consumers == 0)
			{
				WSACleanup();
			}
#endif
		}

		void close() noexcept
		{
			if (fd != -1)
			{
#if SOUP_WINDOWS
				if (encrypted)
				{
					sendCloseNotify();
				}

				closesocket(fd);
#else
				close(fd);
#endif
				fd = -1;
			}
			deleteSecurityContext();
		}

	private:
		void deleteSecurityContext()
		{
#if SOUP_WINDOWS
			if (SecIsValidHandle(&ctx_h))
			{
				sft->DeleteSecurityContext(&ctx_h);
				SecInvalidateHandle(&ctx_h);
			}
#endif
		}

	public:
		[[nodiscard]] bool hasConnection() const noexcept
		{
			return fd != -1;
		}

		bool init(int af)
		{
			close();
			fd = ::socket(af, SOCK_STREAM, 0);
			return fd != -1;
		}

		bool connectSecure(const char* host, uint16_t port) noexcept
		{
			return connectReliable(host, port) && encrypt(host);
		}

		bool connectReliable(const char* host, uint16_t port) noexcept;

		bool connect(const addr_socket& desc) noexcept
		{
			return connect(desc.addr, desc.port);
		}

		bool connect(const addr_ip& ip, uint16_t port) noexcept
		{
			if (ip.isV4())
			{
				if (!init(AF_INET))
				{
					return false;
				}
				sockaddr_in addr{};
				addr.sin_family = AF_INET;
				addr.sin_port = htons(port);
				addr.sin_addr.s_addr = ip.getV4();
				if (::connect(fd, (sockaddr*)&addr, sizeof(addr)) == -1)
				{
					return false;
				}
			}
			else
			{
				if (!init(AF_INET6))
				{
					return false;
				}
				sockaddr_in6 addr{};
				addr.sin6_family = AF_INET6;
				memcpy(&addr.sin6_addr, &ip.data, sizeof(in6_addr));
				addr.sin6_port = htons(port);
				if (::connect(fd, (sockaddr*)&addr, sizeof(addr)) == -1)
				{
					return false;
				}
			}
			return true;
		}

		bool setBlocking(bool blocking = true) noexcept
		{
#if SOUP_WINDOWS
			unsigned long mode = blocking ? 0 : 1;
			return (ioctlsocket(fd, FIONBIO, &mode) == 0) ? true : false;
#else
			int flags = fcntl(fd, F_GETFL, 0);
			if (flags == -1) return false;
			flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
			return (fcntl(fd, F_SETFL, flags) == 0) ? true : false;
#endif
		}

		bool setNonBlocking() noexcept
		{
			return setBlocking(false);
		}

		bool encrypt(const char* server_name) noexcept;

		bool send(const std::string& data) noexcept
		{
			return send(data.data(), (int)data.size());
		}

		bool send(const void* data, int size) noexcept;

	private:
		bool sendUnencrypted(const void* data, int size) noexcept
		{
			return ::send(fd, (const char*)data, size, 0) == size;
		}

	public:
		int recv(void* outData, int max_bytes) noexcept;

	private:
		int recvUnencrypted(void* outData, int max_bytes) noexcept
		{
			return ::recv(fd, (char*)outData, max_bytes, 0);
		}

		[[nodiscard]] std::string recv(int max_bytes) noexcept
		{
			std::string buf(max_bytes, 0);
			int read = ::recv(fd, buf.data(), max_bytes, 0);
			if (read <= 0)
			{
				return std::string{};
			}
			buf.resize(read);
			return buf;
		}

		[[nodiscard]] std::string recvExact(int bytes) noexcept
		{
			std::string buf(bytes, 0);
			char* dp = buf.data();
			while (bytes != 0)
			{
				int read = ::recv(fd, dp, bytes, 0);
				if (read <= 0)
				{
					return std::string{};
				}
				bytes -= read;
				dp += read;
			}
			return buf;
		}

	public:
		bool recvAll(std::string& out) noexcept
		{
			char buf[4096];
			while (true)
			{
				auto res = recv(buf, sizeof(buf));

				// data
				if (res > 0)
				{
					out.append(buf, res);
					continue;
				}

				// closed
				if (res == 0)
				{
					break;
				}

				// error
#if SOUP_WINDOWS
				const auto err = WSAGetLastError();
				if (err == 0)
				{
					break;
				}
				if (err != WSAEWOULDBLOCK)
				{
					return false;
				}
#else
				if (errno != EWOULDBLOCK && errno != EAGAIN)
				{
					return false;
				}
#endif
			}
			return true;
		}

	private:
		void sendCloseNotify() noexcept;
	};
}
