#pragma once

#include "worker.hpp"

#include "base.hpp"
#include "fwd.hpp"

#include <memory>

#if SOUP_WINDOWS
#pragma comment(lib, "Ws2_32.lib")
#include <Winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include "addr_socket.hpp"
#include "capture.hpp"
#include "socket_tls_encrypter.hpp"

namespace soup
{
#pragma pack(push, 1)
	class socket : public worker
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

		addr_socket peer;

		std::string tls_record_buf_data{};
		tls_content_type_t tls_record_buf_content_type;

		socket_tls_encrypter tls_encrypter_send;
		socket_tls_encrypter tls_encrypter_recv;

		socket() noexcept;

		socket(const socket&) = delete;

		socket(socket&& b) noexcept
		{
			onConstruct();
			operator =(std::move(b));
		}

	protected:
		void onConstruct() noexcept;

	public:
		~socket() noexcept;

		void operator =(const socket&) = delete;

		void operator =(socket&& b) noexcept;

		[[nodiscard]] constexpr bool hasConnection() const noexcept
		{
			return fd != -1;
		}

		bool init(int af);

		bool connect(const char* host, uint16_t port) noexcept;
		bool connect(const addr_socket& addr) noexcept;
		bool connect(const addr_ip& ip, uint16_t port) noexcept;

		bool bind6(uint16_t port) noexcept;
		bool bind4(uint16_t port) noexcept;

		[[nodiscard]] socket accept6() noexcept;
		[[nodiscard]] socket accept4() noexcept;

		bool setBlocking(bool blocking = true) noexcept;
		bool setNonBlocking() noexcept;

		void enableCryptoClient(std::string server_name, void(*callback)(socket&, capture&&), capture&& cap = {});
	protected:
		void enableCryptoClientRecvServerHelloDone(std::unique_ptr<socket_tls_handshaker>&& handshaker);

	public:
		void enableCryptoServer(void(*cert_selector)(socket_tls_server_rsa_data& out, const std::string& server_name), void(*callback)(socket&, capture&&), capture&& cap = {});
	
		// Application Layer

		bool send(const std::string& data);

		void recv(void(*callback)(socket&, std::string&&, capture&&), capture&& cap = {});

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

		bool tls_sendHandshake(const std::unique_ptr<socket_tls_handshaker>& handshaker, tls_handshake_type_t handshake_type, const std::string& content);
		bool tls_sendRecord(tls_content_type_t content_type, std::string content);

		void tls_recvHandshake(std::unique_ptr<socket_tls_handshaker>&& handshaker, tls_handshake_type_t expected_handshake_type, void(*callback)(socket&, std::unique_ptr<socket_tls_handshaker>&&, std::string&&), std::string&& pre = {});
		void tls_recvRecord(tls_content_type_t expected_content_type, void(*callback)(socket&, std::string&&, capture&&), capture&& cap = {});
		void tls_recvRecord(void(*callback)(socket&, tls_content_type_t, std::string&&, capture&&), capture&& cap = {});

		void tls_unrecv(tls_content_type_t content_type, std::string&& content) noexcept;

		void tls_close(tls_alert_description_t desc);

		// Transport Layer

		bool transport_send(const std::string& data) const noexcept;
		bool transport_send(const void* data, int size) const noexcept;

		using transport_recv_callback_t = void(*)(socket&, std::string&&, capture&&);

	protected:
		[[nodiscard]] std::string transport_recvCommon(int max_bytes);
	public:
		void transport_recv(int max_bytes, transport_recv_callback_t callback, capture&& cap = {});
		void transport_recvExact(int bytes, transport_recv_callback_t callback, capture&& cap = {}, std::string&& pre = {});

		void transport_close() noexcept;
	};
#pragma pack(pop)
}
