#include "socket.hpp"

#if SOUP_LINUX
#include <fcntl.h>
#include <signal.h>
#include <unistd.h> // close
#endif

#include "aes.hpp"
#include "dns.hpp"
#include "ec.hpp"
#include "named_curves.hpp"
#include "rand.hpp"
#include "socket_tls_handshaker.hpp"
#include "time.hpp"
#include "tls_alert_description.hpp"
#include "tls_certificate.hpp"
#include "tls_client_hello.hpp"
#include "tls_client_hello_ext_elliptic_curves.hpp"
#include "tls_client_hello_ext_server_name.hpp"
#include "tls_content_type.hpp"
#include "tls_encrypted_pre_master_secret.hpp"
#include "tls_extension_type.hpp"
#include "tls_handshake.hpp"
#include "tls_record.hpp"
#include "tls_server_hello.hpp"
#include "tls_server_key_exchange.hpp"

namespace soup
{
#if SOUP_LINUX
	static void sigpipe_handler_proc(int)
	{
	}
#endif

	socket::socket() noexcept
	{
		onConstruct();
#if SOUP_LINUX
		struct sigaction sigpipe_handler;
		sigpipe_handler.sa_handler = &sigpipe_handler_proc;
		sigemptyset(&sigpipe_handler.sa_mask);
		sigpipe_handler.sa_flags = 0;
		sigaction(SIGPIPE, &sigpipe_handler, NULL);
#endif
	}

	void socket::onConstruct() noexcept
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

	socket::~socket() noexcept
	{
		close();
#if SOUP_WINDOWS
		if (--wsa_consumers == 0)
		{
			WSACleanup();
		}
#endif
	}

	void socket::operator=(socket&& b) noexcept
	{
		fd = b.fd;
		peer = std::move(b.peer);
		on_data_available = b.on_data_available;
		on_data_available_capture = std::move(b.on_data_available_capture);
		tls_record_buf_data = std::move(b.tls_record_buf_data);
		tls_record_buf_content_type = b.tls_record_buf_content_type;
		tls_encrypter_send = std::move(b.tls_encrypter_send);
		tls_encrypter_recv = std::move(b.tls_encrypter_recv);

		b.fd = -1;
	}

	bool socket::init(int af)
	{
		close();
		fd = ::socket(af, SOCK_STREAM, 0);
		return fd != -1;
	}

	bool socket::connect(const char* host, uint16_t port) noexcept
	{
		auto res = dns::lookupIPv4(host);
		if (!res.empty() && connect(rand(res), port))
		{
			return true;
		}
		res = dns::lookupIPv6(host);
		if (!res.empty() && connect(rand(res), port))
		{
			return true;
		}
		return false;
	}

	bool socket::connect(const addr_socket& addr) noexcept
	{
		peer = addr;
		if (addr.ip.isV4())
		{
			if (!init(AF_INET))
			{
				return false;
			}
			sockaddr_in sa{};
			sa.sin_family = AF_INET;
			sa.sin_port = htons(addr.port);
			sa.sin_addr.s_addr = addr.ip.getV4();
			if (::connect(fd, (sockaddr*)&sa, sizeof(sa)) == -1)
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
			sockaddr_in6 sa{};
			sa.sin6_family = AF_INET6;
			memcpy(&sa.sin6_addr, &addr.ip.data, sizeof(in6_addr));
			sa.sin6_port = htons(addr.port);
			if (::connect(fd, (sockaddr*)&sa, sizeof(sa)) == -1)
			{
				return false;
			}
		}
		return setNonBlocking();
	}

	bool socket::connect(const addr_ip& ip, uint16_t port) noexcept
	{
		return connect(addr_socket(ip, port));
	}

	bool socket::bind6(uint16_t port) noexcept
	{
		if (!init(AF_INET6))
		{
			return false;
		}
		peer.ip.reset();
		peer.port = port;
		sockaddr_in6 addr{};
		addr.sin6_family = AF_INET6;
		addr.sin6_port = htons(port);
		return bind(fd, (sockaddr*)&addr, sizeof(addr)) != -1
			&& listen(fd, 100) != -1
			&& setNonBlocking();
	}

	bool socket::bind4(uint16_t port) noexcept
	{
		if (!init(AF_INET))
		{
			return false;
		}
		peer.ip.reset();
		peer.port = port;
		sockaddr_in addr{};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		return bind(fd, (sockaddr*)&addr, sizeof(addr)) != -1
			&& listen(fd, 100) != -1
			&& setNonBlocking();
	}

#if SOUP_WINDOWS
	using socklen_t = int;
#endif

	socket socket::accept6() noexcept
	{
		socket res{};
		sockaddr_in6 addr;
		socklen_t addrlen = sizeof(addr);
		res.fd = ::accept(fd, (sockaddr*)&addr, &addrlen);
		if (res.hasConnection())
		{
			memcpy(&res.peer.ip, &addr.sin6_addr, sizeof(addr.sin6_addr));
			res.peer.port = addr.sin6_port;
		}
		return res;
	}

	socket socket::accept4() noexcept
	{
		socket res{};
		sockaddr_in addr;
		socklen_t addrlen = sizeof(addr);
		res.fd = ::accept(fd, (sockaddr*)&addr, &addrlen);
		if (res.hasConnection())
		{
			res.peer.ip = addr.sin_addr.s_addr;
			res.peer.port = addr.sin_port;
		}
		return res;
	}

	bool socket::setBlocking(bool blocking) noexcept
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

	bool socket::setNonBlocking() noexcept
	{
		return setBlocking(false);
	}

	bool socket::fireDataAvailable()
	{
		if (on_data_available)
		{
			return on_data_available(*this);
		}
		return true;
	}

	void socket::enableCryptoClient(std::string server_name, void(*callback)(socket&, capture&&), capture&& cap)
	{
		auto handshaker = std::make_unique<socket_tls_handshaker>(
			callback,
			std::move(cap)
		);

		tls_client_hello hello;
		hello.random.time = time::unixSeconds();
		rand.fill(hello.random.random);
		handshaker->client_random = hello.random.toBinary();
		hello.cipher_suites = {
			TLS_RSA_WITH_AES_256_CBC_SHA256,
			TLS_RSA_WITH_AES_128_CBC_SHA256,
			TLS_RSA_WITH_AES_256_CBC_SHA,
			TLS_RSA_WITH_AES_128_CBC_SHA,
			TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA,
			TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256,
			TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA,
		};
		hello.compression_methods = { 0 };

		{
			tls_client_hello_ext_server_name ext_server_name{};
			ext_server_name.host_name = std::move(server_name);

			hello.extensions.add(tls_extension_type::server_name, ext_server_name);
		}

		{
			tls_client_hello_ext_elliptic_curves ext_elliptic_curves{};
			ext_elliptic_curves.named_curves = {
				named_curves::x25519,
			};

			hello.extensions.add(tls_extension_type::elliptic_curves, ext_elliptic_curves);
		}

		if (tls_sendHandshake(handshaker, tls_handshake::client_hello, hello.toBinary()))
		{
			tls_recvHandshake(std::move(handshaker), tls_handshake::server_hello, [](socket& s, std::unique_ptr<socket_tls_handshaker>&& handshaker, std::string&& data)
			{
				tls_server_hello shello;
				if (!shello.fromBinary(data))
				{
					s.tls_close(tls_alert_description::decode_error);
					return;
				}
				handshaker->cipher_suite = shello.cipher_suite;
				handshaker->server_random = shello.random.toBinary();

				s.tls_recvHandshake(std::move(handshaker), tls_handshake::certificate, [](socket& s, std::unique_ptr<socket_tls_handshaker>&& handshaker, std::string&& data)
				{
					tls_certificate cert;
					if (!cert.fromBinary(data))
					{
						s.tls_close(tls_alert_description::decode_error);
						return;
					}
					for (const auto& asn1_cert : cert.asn1_certs)
					{
						x509_certificate xcert{};
						if (!xcert.fromBinary(asn1_cert))
						{
							s.tls_close(tls_alert_description::bad_certificate);
							return;
						}
						handshaker->certchain.emplace_back(std::move(xcert));
					}
					if (handshaker->certchain.empty())
					{
						s.tls_close(tls_alert_description::bad_certificate);
						return;
					}
					// TODO: Validate certchain

					switch (handshaker->cipher_suite)
					{
					default:
						s.enableCryptoClientRecvServerHelloDone(std::move(handshaker));
						break;

					case TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA:
					case TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256:
					case TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA:
						s.tls_recvHandshake(std::move(handshaker), tls_handshake::server_key_exchange, [](socket& s, std::unique_ptr<socket_tls_handshaker>&& handshaker, std::string&& data)
						{
							tls_server_key_exchange ske;
							if (!ske.fromBinary(data))
							{
								s.tls_close(tls_alert_description::decode_error);
								return;
							}
							if (ske.named_curve != named_curves::x25519
								|| ske.point.size() != ec::X25519_KEY_SIZE
								)
							{
								s.tls_close(tls_alert_description::handshake_failure);
								return;
							}
							// TODO: Validate signature
							handshaker->server_x25519_public_key = std::move(ske.point);

							s.enableCryptoClientRecvServerHelloDone(std::move(handshaker));
						});
						break;
					}
				});
			});
		}
	}

	void socket::enableCryptoClientRecvServerHelloDone(std::unique_ptr<socket_tls_handshaker>&& handshaker)
	{
		tls_recvHandshake(std::move(handshaker), tls_handshake::server_hello_done, [](socket& s, std::unique_ptr<socket_tls_handshaker>&& handshaker, std::string&& data)
		{
			std::string cke{};
			if (handshaker->server_x25519_public_key.empty())
			{
				std::string pms{};
				pms.reserve(48);
				pms.push_back('\3');
				pms.push_back('\3');
				for (auto i = 0; i != 46; ++i)
				{
					pms.push_back(rand.ch());
				}

				tls_encrypted_pre_master_secret epms{};
				epms.data = handshaker->certchain.at(0).key.encryptPkcs1(pms).toBinary();
				cke = epms.toBinary();

				handshaker->pre_master_secret = std::make_unique<promise<std::string>>(std::move(pms));
			}
			else
			{
				uint8_t my_priv[ec::X25519_KEY_SIZE];
				ec::curve25519_generatePrivate(my_priv);

				uint8_t their_pub[ec::X25519_KEY_SIZE];
				memcpy(their_pub, handshaker->server_x25519_public_key.data(), sizeof(their_pub));

				uint8_t shared_secret[ec::X25519_SHARED_SIZE];
				ec::x25519(shared_secret, my_priv, their_pub);
				handshaker->pre_master_secret = std::make_unique<promise<std::string>>(std::string((const char*)shared_secret, sizeof(shared_secret)));

				uint8_t my_pub[ec::X25519_KEY_SIZE];
				ec::curve25519_derivePublic(my_pub, my_priv);

				cke = std::string(1, (char)sizeof(my_pub));
				cke.append((const char*)my_pub, sizeof(my_pub));
			}
			if (s.tls_sendHandshake(handshaker, tls_handshake::client_key_exchange, std::move(cke))
				&& s.tls_sendRecord(tls_content_type::change_cipher_spec, "\1")
				)
			{
				handshaker->getKeys(s.tls_encrypter_send.mac_key, handshaker->pending_recv_encrypter.mac_key, s.tls_encrypter_send.cipher_key, handshaker->pending_recv_encrypter.cipher_key);
				if (s.tls_sendHandshake(handshaker, tls_handshake::finished, handshaker->getClientFinishVerifyData()))
				{
					s.tls_recvRecord(tls_content_type::change_cipher_spec, [](socket& s, std::string&& data, capture&& cap)
					{
						std::unique_ptr<socket_tls_handshaker> handshaker = std::move(cap.get<std::unique_ptr<socket_tls_handshaker>>());

						s.tls_encrypter_recv = std::move(handshaker->pending_recv_encrypter);

						handshaker->expected_finished_verify_data = handshaker->getServerFinishVerifyData();

						s.tls_recvHandshake(std::move(handshaker), tls_handshake::finished, [](socket& s, std::unique_ptr<socket_tls_handshaker>&& handshaker, std::string&& data)
						{
							if (data != handshaker->expected_finished_verify_data)
							{
								s.tls_close(tls_alert_description::decrypt_error);
								return;
							}
							handshaker->callback(s, std::move(handshaker->callback_capture));
						});
					}, std::move(handshaker));
				}
			}
		});
	}

	[[nodiscard]] static bool tls_serverSupportsCipherSuite(uint16_t cs) noexcept
	{
		switch (cs)
		{
		case TLS_RSA_WITH_AES_128_CBC_SHA:
		case TLS_RSA_WITH_AES_256_CBC_SHA:
		case TLS_RSA_WITH_AES_128_CBC_SHA256:
		case TLS_RSA_WITH_AES_256_CBC_SHA256:
			return true;
		}
		return false;
	}

	struct capture_decrypt_pre_master_secret
	{
		socket_tls_handshaker* handshaker;
		bigint data;
	};

	void socket::enableCryptoServer(void(*cert_selector)(socket_tls_server_rsa_data& out, const std::string& server_name), void(*callback)(socket&, capture&&), capture&& cap)
	{
		auto handshaker = std::make_unique<socket_tls_handshaker>(
			callback,
			std::move(cap)
		);
		handshaker->cert_selector = cert_selector;
		tls_recvHandshake(std::move(handshaker), tls_handshake::client_hello, [](socket& s, std::unique_ptr<socket_tls_handshaker>&& handshaker, std::string&& data)
		{
			{
				tls_client_hello hello;
				if (!hello.fromBinary(data))
				{
					s.tls_close(tls_alert_description::decode_error);
					return;
				}
				for (const auto& cs : hello.cipher_suites)
				{
					if (tls_serverSupportsCipherSuite(cs))
					{
						handshaker->cipher_suite = cs;
						break;
					}
				}

				std::string server_name{};
				for (const auto& ext : hello.extensions.extensions)
				{
					if (ext.id == tls_extension_type::server_name)
					{
						tls_client_hello_ext_server_name ext_server_name;
						if (ext_server_name.fromBinary(ext.data))
						{
							server_name = std::move(ext_server_name.host_name);
						}
						break;
					}
				}
				handshaker->cert_selector(handshaker->rsa_data, server_name);
				if (handshaker->rsa_data.der_encoded_certchain.empty())
				{
					s.tls_close(tls_alert_description::handshake_failure);
					return;
				}

				handshaker->client_random = hello.random.toBinary();
			}

			{
				tls_server_hello shello{};
				shello.random.time = time::unixSeconds();
				rand.fill(shello.random.random);
				handshaker->server_random = shello.random.toBinary();
				shello.cipher_suite = handshaker->cipher_suite;
				shello.compression_method = 0;
				if (!s.tls_sendHandshake(handshaker, tls_handshake::server_hello, shello.toBinary()))
				{
					return;
				}
			}

			{
				tls_certificate tcert;
				tcert.asn1_certs = std::move(handshaker->rsa_data.der_encoded_certchain);
				if (!s.tls_sendHandshake(handshaker, tls_handshake::certificate, tcert.toBinary()))
				{
					return;
				}
			}

			if (!s.tls_sendHandshake(handshaker, tls_handshake::server_hello_done, {}))
			{
				return;
			}

			s.tls_recvHandshake(std::move(handshaker), tls_handshake::client_key_exchange, [](socket& s, std::unique_ptr<socket_tls_handshaker>&& handshaker, std::string&& data)
			{
				if (data.size() <= 2)
				{
					s.tls_close(tls_alert_description::decode_error);
					return;
				}
				data.erase(0, 2); // length prefix

				handshaker->pre_master_secret = std::make_unique<promise<std::string>>([](capture&& _cap)
				{
					auto& cap = _cap.get<capture_decrypt_pre_master_secret>();
					// BUG: This crypto operation is slow as fuck
					return cap.handshaker->rsa_data.private_key.decryptPkcs1(cap.data);
				}, capture_decrypt_pre_master_secret{
					handshaker.get(),
					bigint::fromBinary(data)
				});

				s.tls_recvRecord(tls_content_type::change_cipher_spec, [](socket& s, std::string&& data, capture&& cap)
				{
					if (!s.tls_sendRecord(tls_content_type::change_cipher_spec, "\1"))
					{
						return;
					}

					std::unique_ptr<socket_tls_handshaker> handshaker = std::move(cap.get<std::unique_ptr<socket_tls_handshaker>>());

					handshaker->getKeys(s.tls_encrypter_recv.mac_key, s.tls_encrypter_send.mac_key, s.tls_encrypter_recv.cipher_key, s.tls_encrypter_send.cipher_key);

					handshaker->expected_finished_verify_data = handshaker->getClientFinishVerifyData();

					s.tls_recvHandshake(std::move(handshaker), tls_handshake::finished, [](socket& s, std::unique_ptr<socket_tls_handshaker>&& handshaker, std::string&& data)
					{
						if (data != handshaker->expected_finished_verify_data)
						{
							s.tls_close(tls_alert_description::decrypt_error);
							return;
						}

						if (s.tls_sendHandshake(handshaker, tls_handshake::finished, handshaker->getServerFinishVerifyData()))
						{
							handshaker->callback(s, std::move(handshaker->callback_capture));
						}
					});
				}, std::move(handshaker));
			});
		});
	}

	bool socket::send(const std::string& data)
	{
		if (tls_encrypter_send.isActive())
		{
			return tls_sendRecord(tls_content_type::application_data, data);
		}
		return transport_send(data);
	}

	struct capture_socket_recv
	{
		void(*callback)(socket&, std::string&&, capture&&);
		capture cap;
	};

	void socket::recv(void(*callback)(socket&, std::string&&, capture&&), capture&& cap)
	{
		capture_socket_recv inner_cap{
			callback,
			std::move(cap)
		};
		auto inner_callback = [](socket& s, std::string&& data, capture&& _cap)
		{
			auto& cap = _cap.get<capture_socket_recv>();
			cap.callback(s, std::move(data), std::move(cap.cap));
		};
		if (tls_encrypter_recv.isActive())
		{
			tls_recvRecord(tls_content_type::application_data, inner_callback, std::move(inner_cap));
		}
		else
		{
			transport_recv(8192, inner_callback, std::move(inner_cap));
		}
	}

	void socket::close()
	{
		if (tls_encrypter_send.isActive())
		{
			tls_close(tls_alert_description::close_notify);
		}
		else
		{
			transport_close();
		}
	}

	bool socket::tls_sendHandshake(const std::unique_ptr<socket_tls_handshaker>& handshaker, tls_handshake_type_t handshake_type, const std::string& content)
	{
		return tls_sendRecord(tls_content_type::handshake, handshaker->pack(handshake_type, content));
	}

	bool socket::tls_sendRecord(tls_content_type_t content_type, std::string content)
	{
		if (tls_encrypter_send.isActive())
		{
			content = tls_encrypter_send.encrypt(content_type, content);
		}
		tls_record record{};
		record.content_type = content_type;
		record.length = content.size();
		auto data = record.toBinary();
		data.append(content);
		return transport_send(data);
	}

	struct capture_socket_tls_recv_handshake
	{
		std::unique_ptr<socket_tls_handshaker> handshaker;
		tls_handshake_type_t expected_handshake_type;
		void(*callback)(socket&, std::unique_ptr<socket_tls_handshaker>&&, std::string&&);
		std::string pre;
		bool is_new_bytes = false;
	};

	void socket::tls_recvHandshake(std::unique_ptr<socket_tls_handshaker>&& handshaker, tls_handshake_type_t expected_handshake_type, void(*callback)(socket&, std::unique_ptr<socket_tls_handshaker>&&, std::string&&), std::string&& pre)
	{
		capture_socket_tls_recv_handshake cap{
			std::move(handshaker),
			expected_handshake_type,
			callback,
			std::move(pre)
		};

		auto record_callback = [](socket& s, tls_content_type_t content_type, std::string&& data, capture&& _cap)
		{
			if (content_type != tls_content_type::handshake)
			{
				s.tls_close(tls_alert_description::unexpected_message);
				return;
			}

			auto& cap = _cap.get<capture_socket_tls_recv_handshake>();

			if (cap.is_new_bytes)
			{
				cap.handshaker->layer_bytes.append(data);
			}

			if (!cap.pre.empty())
			{
				data.insert(0, cap.pre);
			}

			tls_handshake hs;
			if (!hs.fromBinary(data))
			{
				s.tls_close(tls_alert_description::decode_error);
				return;
			}

			if (hs.handshake_type != cap.expected_handshake_type)
			{
				s.tls_close(tls_alert_description::unexpected_message);
				return;
			}

			if (hs.length > (data.size() - 4))
			{
				s.tls_recvHandshake(std::move(cap.handshaker), cap.expected_handshake_type, cap.callback, std::move(data));
				return;
			}

			data.erase(0, 4);

			if (data.size() > hs.length)
			{
				s.tls_unrecv(tls_content_type::handshake, data.substr(hs.length));
				data.erase(hs.length);
			}

			cap.callback(s, std::move(cap.handshaker), std::move(data));
		};

		if (!tls_record_buf_data.empty())
		{
			std::string data = std::move(tls_record_buf_data);
			tls_record_buf_data.clear();
			record_callback(*this, tls_record_buf_content_type, std::move(data), std::move(cap));
			return;
		}

		cap.is_new_bytes = true;
		tls_recvRecord(record_callback, std::move(cap));
	}

	struct capture_socket_tls_recv_record_expect
	{
		tls_content_type_t expected_content_type;
		void(*callback)(socket&, std::string&&, capture&&);
		capture cap;
	};

	void socket::tls_recvRecord(tls_content_type_t expected_content_type, void(*callback)(socket&, std::string&&, capture&&), capture&& cap)
	{
		tls_recvRecord([](socket& s, tls_content_type_t content_type, std::string&& data, capture&& _cap)
		{
			auto& cap = _cap.get<capture_socket_tls_recv_record_expect>();
			if (content_type != cap.expected_content_type)
			{
				s.tls_close(tls_alert_description::unexpected_message);
				return;
			}
			cap.callback(s, std::move(data), std::move(cap.cap));
		}, capture_socket_tls_recv_record_expect{
			expected_content_type,
			callback,
			std::move(cap)
		});
	}

	struct capture_socket_tls_recv_record_1
	{
		void(*callback)(socket&, tls_content_type_t, std::string&&, capture&&);
		capture cap;
	};

	struct capture_socket_tls_recv_record_2
	{
		capture_socket_tls_recv_record_1 prev;
		tls_content_type_t content_type;
	};

	void socket::tls_recvRecord(void(*callback)(socket&, tls_content_type_t, std::string&&, capture&&), capture&& cap)
	{
		if (!tls_record_buf_data.empty())
		{
			std::string data = std::move(tls_record_buf_data);
			tls_record_buf_data.clear();
			callback(*this, tls_record_buf_content_type, std::move(data), std::move(cap));
			return;
		}
		transport_recvExact(5, [](socket& s, std::string&& data, capture&& cap)
		{
			tls_record record{};
			if (!record.fromBinary(data))
			{
				s.tls_close(tls_alert_description::decode_error);
				return;
			}
			s.transport_recvExact(record.length, [](socket& s, std::string&& data, capture&& _cap)
			{
				auto& cap = _cap.get<capture_socket_tls_recv_record_2>();
				if (s.tls_encrypter_recv.isActive())
				{
					constexpr auto cipher_bytes = 16;
					const auto mac_length = s.tls_encrypter_recv.mac_key.size();

					if ((data.size() % cipher_bytes) != 0
						|| data.size() < (cipher_bytes + mac_length)
						)
					{
						s.tls_close(tls_alert_description::bad_record_mac);
						return;
					}

					auto iv = data.substr(0, cipher_bytes);
					data.erase(0, cipher_bytes);
					data = aes::decryptCBC(data, s.tls_encrypter_recv.cipher_key, iv);

					std::string mac{};

					bool pad_mismatch = false;
					uint8_t pad_len = data.at(data.size() - 1);
					for (auto it = (data.end() - (pad_len + 1)); it != (data.end() - 1); ++it)
					{
						if (*it != pad_len)
						{
							pad_mismatch = true;
						}
					}
					if (data.size() >= pad_len)
					{
						data.erase(data.size() - (pad_len + 1));

						if (data.size() > mac_length)
						{
							mac = data.substr(data.size() - mac_length);
							data.erase(data.size() - mac_length);
						}
					}

					if (s.tls_encrypter_recv.calculateMac(cap.content_type, data) != mac
						|| pad_mismatch
						)
					{
						s.tls_close(tls_alert_description::bad_record_mac);
						return;
					}
				}
				cap.prev.callback(s, cap.content_type, std::move(data), std::move(cap.prev.cap));
			}, capture_socket_tls_recv_record_2{
				std::move(cap.get<capture_socket_tls_recv_record_1>()),
				record.content_type
			});
		}, capture_socket_tls_recv_record_1{
			callback,
			std::move(cap)
		});
	}

	void socket::tls_unrecv(tls_content_type_t content_type, std::string&& content) noexcept
	{
		tls_record_buf_data = std::move(content);
		tls_record_buf_content_type = content_type;
	}

	void socket::tls_close(tls_alert_description_t desc)
	{
		if (hasConnection())
		{
			{
				std::string bin(1, '\2'); // fatal
				bin.push_back((char)desc); static_assert(sizeof(tls_alert_description_t) == sizeof(char));
				tls_sendRecord(tls_content_type::alert, bin);
			}

			tls_encrypter_send.reset();
			tls_encrypter_recv.reset();

			transport_close();
		}
	}

	bool socket::transport_send(const std::string& data) const noexcept
	{
		return transport_send(data.data(), data.size());
	}

	bool socket::transport_send(const void* data, int size) const noexcept
	{
		return ::send(fd, (const char*)data, size, 0) == size;
	}

	std::string socket::transport_recvCommon(int max_bytes)
	{
		std::string buf(max_bytes, '\0');
		auto res = ::recv(fd, buf.data(), max_bytes, 0);
		if (res > 0)
		{
			buf.resize(res);
			return buf;
		}
//		/*else*/ if (res == 0
//			|| (/*res == -1 &&*/
//#if SOUP_WINDOWS
//				WSAGetLastError() != WSAEWOULDBLOCK
//#else
//				errno != EWOULDBLOCK && errno != EAGAIN
//#endif
//				)
//			)
//		{
//			close();
//		}
		return {};
	}

	struct capture_socket_transport_recv
	{
		int bytes;
		socket::transport_recv_callback_t callback;
		capture cap;
	};

	void socket::transport_recv(int max_bytes, transport_recv_callback_t callback, capture&& cap)
	{
		if (auto buf = transport_recvCommon(max_bytes); !buf.empty())
		{
			callback(*this, std::move(buf), std::move(cap));
			return;
		}
		on_data_available = [](socket& s)
		{
			auto& c = s.on_data_available_capture.get<capture_socket_transport_recv>();
			s.transport_recv(c.bytes, c.callback, std::move(c.cap));
			return true;
		};
		on_data_available_capture = capture_socket_transport_recv{ max_bytes, callback, std::move(cap) };
	}

	struct capture_socket_transport_recv_exact : public capture_socket_transport_recv
	{
		std::string buf;

		capture_socket_transport_recv_exact(int bytes, socket::transport_recv_callback_t callback, capture&& cap, std::string&& buf)
			: capture_socket_transport_recv{ bytes, callback, std::move(cap) }, buf(std::move(buf))
		{
		}
	};

	void socket::transport_recvExact(int bytes, transport_recv_callback_t callback, capture&& cap, std::string&& pre)
	{
		auto remainder = (bytes - pre.size());
		if (auto buf = transport_recvCommon(remainder); !buf.empty())
		{
			pre.append(buf);
		}
		if (pre.size() == bytes)
		{
			callback(*this, std::move(pre), std::move(cap));
			return;
		}
		on_data_available = [](socket& s)
		{
			auto& c = s.on_data_available_capture.get<capture_socket_transport_recv_exact>();
			s.transport_recvExact(c.bytes, c.callback, std::move(c.cap), std::move(c.buf));
			return true;
		};
		on_data_available_capture = capture_socket_transport_recv_exact(bytes, callback, std::move(cap), std::move(pre));
	}

	void socket::transport_close() noexcept
	{
		if (hasConnection())
		{
#if SOUP_WINDOWS
			::closesocket(fd);
#else
			::close(fd);
#endif
			fd = -1;
		}
	}
}
