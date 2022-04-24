#pragma once

#include "Worker.hpp"

#include "base.hpp"
#include "fwd.hpp"

#if SOUP_WINDOWS
#pragma comment(lib, "Ws2_32.lib")
#include <Winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include "SocketAddr.hpp"
#include "Capture.hpp"
#include "SocketTlsEncrypter.hpp"
#include "UniquePtr.hpp"

namespace soup
{
#pragma pack(push, 1)
	class Socket : public Worker
	{
	public:
#if SOUP_WINDOWS
		inline static size_t wsa_consumers = 0;
#endif

#if SOUP_WINDOWS
		using fd_t = ::SOCKET;
#else
		using fd_t = int;
#endif
		fd_t fd = -1;
		SocketAddr peer;
		Capture user_data;
		bool remote_closed = false;

		std::string tls_record_buf_data{};
		TlsContentType_t tls_record_buf_content_type;

		SocketTlsEncrypter tls_encrypter_send;
		SocketTlsEncrypter tls_encrypter_recv;

		Socket() noexcept;

		Socket(const Socket&) = delete;

		Socket(Socket&& b) noexcept
		{
			onConstruct();
			operator =(std::move(b));
		}

	protected:
		void onConstruct() noexcept;

	public:
		~Socket() noexcept;

		void operator =(const Socket&) = delete;

		void operator =(Socket&& b) noexcept;

		[[nodiscard]] constexpr bool hasConnection() const noexcept
		{
			return fd != -1;
		}

		bool init(int af);

		bool connect(const std::string& host, uint16_t port) noexcept; // blocking
		bool connect(const char* host, uint16_t port) noexcept; // blocking
		bool connect(const SocketAddr& addr) noexcept; // blocking
		bool connect(const IpAddr& ip, uint16_t port) noexcept; // blocking

		template <typename T = int>
		bool setOpt(int name, T&& val) noexcept
		{
#if SOUP_WINDOWS
			return setsockopt(fd, SOL_SOCKET, name, (const char*)&val, sizeof(T)) != -1;
#else
			return setsockopt(fd, SOL_SOCKET, name, &val, sizeof(T)) != -1;
#endif
		}

		bool bind6(uint16_t port) noexcept;
		bool bind4(uint16_t port) noexcept;

		[[nodiscard]] Socket accept6() noexcept;
		[[nodiscard]] Socket accept4() noexcept;

		bool setBlocking(bool blocking = true) noexcept;
		bool setNonBlocking() noexcept;

		static bool trustAllCertchainsWithNoChecksWhatsoever_ThisIsNotAJoke_IfYouCareYouShouldLookIntoThis(const Certchain&, const std::string&);

		void enableCryptoClient(std::string server_name, void(*callback)(Socket&, Capture&&), Capture&& cap = {}, bool(*certchain_validator)(const Certchain&, const std::string& server_name) = &trustAllCertchainsWithNoChecksWhatsoever_ThisIsNotAJoke_IfYouCareYouShouldLookIntoThis);
	protected:
		void enableCryptoClientRecvServerHelloDone(UniquePtr<SocketTlsHandshaker>&& handshaker);

	public:
		void enableCryptoServer(void(*cert_selector)(TlsServerRsaData& out, const std::string& server_name), void(*callback)(Socket&, Capture&&), Capture&& cap = {}, void(*on_client_hello)(Socket&, TlsClientHello&&) = nullptr);

		// Application Layer

		[[nodiscard]] bool isEncrypted() const noexcept;

		template <typename T>
		[[nodiscard]] T& getUserData() // Allows you to associate custom data with a socket. Note that all calls to getUserData on the same socket must use the same T.
		{
			if (!user_data)
			{
				user_data = T{};
			}
			return user_data.get<T>();
		}

		bool send(const std::string& data);

		void recv(void(*callback)(Socket&, std::string&&, Capture&&), Capture&& cap = {});

		/*[[nodiscard]] std::string recvExact(int bytes) noexcept
		{
			std::string buf(bytes, 0);
			char* dp = buf.data();
			while (bytes != 0)
			{
				int read = bytes;
				if (!transport_recv(dp, read))
				{
					return {};
				}
				bytes -= read;
				dp += read;
			}
			return buf;
		}*/

		void close();

		// TLS - Crypto Layer

		bool tls_sendHandshake(const UniquePtr<SocketTlsHandshaker>& handshaker, TlsHandshakeType_t handshake_type, const std::string& content);
		bool tls_sendRecord(TlsContentType_t content_type, std::string content);

		void tls_recvHandshake(UniquePtr<SocketTlsHandshaker>&& handshaker, TlsHandshakeType_t expected_handshake_type, void(*callback)(Socket&, UniquePtr<SocketTlsHandshaker>&&, std::string&&), std::string&& pre = {});
		void tls_recvRecord(TlsContentType_t expected_content_type, void(*callback)(Socket&, std::string&&, Capture&&), Capture&& cap = {});
		void tls_recvRecord(void(*callback)(Socket&, TlsContentType_t, std::string&&, Capture&&), Capture&& cap = {});

		void tls_unrecv(TlsContentType_t content_type, std::string&& content) noexcept;

		void tls_close(TlsAlertDescription_t desc);

		// Transport Layer

		bool transport_send(const std::string& data) const noexcept;
		bool transport_send(const void* data, int size) const noexcept;

		using transport_recv_callback_t = void(*)(Socket&, std::string&&, Capture&&);

		[[nodiscard]] bool transport_hasData() const;

	protected:
		[[nodiscard]] std::string transport_recvCommon(int max_bytes);
	public:
		void transport_recv(int max_bytes, transport_recv_callback_t callback, Capture&& cap = {});
		void transport_recvExact(int bytes, transport_recv_callback_t callback, Capture&& cap = {}, std::string&& pre = {});

		void transport_close() noexcept;
	};
#pragma pack(pop)
}
