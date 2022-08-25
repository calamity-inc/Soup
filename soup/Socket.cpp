#include "Socket.hpp"

#if !SOUP_WASM

#if SOUP_LINUX
#include <fcntl.h>
#include <unistd.h> // close

#include "signal.hpp"
#endif

#include "aes.hpp"
#include "dnsOsResolver.hpp"
#include "ec.hpp"
#include "NamedCurves.hpp"
#include "rand.hpp"
#include "SocketTlsHandshaker.hpp"
#include "time.hpp"
#include "TlsAlertDescription.hpp"
#include "TlsCertificate.hpp"
#include "TlsClientHello.hpp"
#include "TlsClientHelloExtEllipticCurves.hpp"
#include "TlsClientHelloExtServerName.hpp"
#include "TlsContentType.hpp"
#include "TlsEncryptedPreMasterSecret.hpp"
#include "TlsExtensionType.hpp"
#include "TlsHandshake.hpp"
#include "TlsRecord.hpp"
#include "TlsServerHello.hpp"
#include "TlsServerKeyExchange.hpp"

namespace soup
{
#if !SOUP_WINDOWS
	static void sigpipe_handler_proc(int)
	{
	}
#endif

	Socket::Socket() noexcept
	{
		onConstruct();
	}

	void Socket::onConstruct() noexcept
	{
#if SOUP_WINDOWS
		if (wsa_consumers++ == 0)
		{
			WSADATA wsaData;
			WORD wVersionRequested = MAKEWORD(2, 2);
			WSAStartup(wVersionRequested, &wsaData);
		}
#else
		if (!registered_sigpipe_handler)
		{
			registered_sigpipe_handler = true;
			signal::handle(SIGPIPE, &sigpipe_handler_proc);
		}
#endif
	}

	Socket::~Socket() noexcept
	{
		close();
#if SOUP_WINDOWS
		if (--wsa_consumers == 0)
		{
			WSACleanup();
		}
#endif
	}

	void Socket::operator=(Socket&& b) noexcept
	{
		Worker::operator=(std::move(b));

		fd = b.fd;
		peer = std::move(b.peer);
		custom_data = std::move(b.custom_data);
		remote_closed = b.remote_closed;

		tls_record_buf = std::move(b.tls_record_buf);
		tls_encrypter_send = std::move(b.tls_encrypter_send);
		tls_encrypter_recv = std::move(b.tls_encrypter_recv);

		b.fd = -1;
	}

	bool Socket::init(int af)
	{
		close();
		fd = ::socket(af, SOCK_STREAM, 0);
		return fd != -1;
	}

	bool Socket::connect(const std::string& host, uint16_t port) noexcept
	{
		dnsOsResolver resolver;
		auto res = resolver.lookupIPv4(host);
		if (!res.empty() && connect(rand(res), port))
		{
			return true;
		}
		res = resolver.lookupIPv6(host);
		if (!res.empty() && connect(rand(res), port))
		{
			return true;
		}
		return false;
	}

	bool Socket::connect(const SocketAddr& addr) noexcept
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

	bool Socket::connect(const IpAddr& ip, uint16_t port) noexcept
	{
		return connect(SocketAddr(ip, port));
	}

	bool Socket::bind6(uint16_t port) noexcept
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
		return setOpt<int>(SO_REUSEADDR, 1)
			&& bind(fd, (sockaddr*)&addr, sizeof(addr)) != -1
			&& listen(fd, 100) != -1
			&& setNonBlocking();
	}

	bool Socket::bind4(uint16_t port) noexcept
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
		return setOpt<int>(SO_REUSEADDR, 1)
			&& bind(fd, (sockaddr*)&addr, sizeof(addr)) != -1
			&& listen(fd, 100) != -1
			&& setNonBlocking();
	}

#if SOUP_WINDOWS
	using socklen_t = int;
#endif

	Socket Socket::accept6() noexcept
	{
		Socket res{};
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

	Socket Socket::accept4() noexcept
	{
		Socket res{};
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

	bool Socket::setBlocking(bool blocking) noexcept
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

	bool Socket::setNonBlocking() noexcept
	{
		return setBlocking(false);
	}

	bool Socket::trustAllCertchainsWithNoChecksWhatsoever_ThisIsNotAJoke_IfYouCareYouShouldLookIntoThis(const X509Certchain&, const std::string&)
	{
		// certchain is already decently implemented, but there's a few flaws:
		// - no ECC support (big deal since cloudflare is basically ecc only now)
		// - no built-in trust stores (and what would be a good default? not all machines running this code can be trusted, to be quite honest.)
		return true;
	}

	void Socket::enableCryptoClient(std::string server_name, void(*callback)(Socket&, Capture&&), Capture&& cap, bool(*certchain_validator)(const X509Certchain&, const std::string& server_name))
	{
		auto handshaker = make_unique<SocketTlsHandshaker>(
			callback,
			std::move(cap)
		);
		handshaker->server_name = std::move(server_name);
		handshaker->certchain_validator = certchain_validator;

		TlsClientHello hello;
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
			TlsClientHelloExtServerName ext_server_name{};
			ext_server_name.host_name = handshaker->server_name;

			hello.extensions.add(TlsExtensionType::server_name, ext_server_name);
		}

		{
			TlsClientHelloExtEllipticCurves ext_elliptic_curves{};
			ext_elliptic_curves.named_curves = {
				NamedCurves::x25519,
			};

			hello.extensions.add(TlsExtensionType::elliptic_curves, ext_elliptic_curves);
		}

		if (tls_sendHandshake(handshaker, TlsHandshake::client_hello, hello.toBinary()))
		{
			tls_recvHandshake(std::move(handshaker), TlsHandshake::server_hello, [](Socket& s, UniquePtr<SocketTlsHandshaker>&& handshaker, std::string&& data)
			{
				TlsServerHello shello;
				if (!shello.fromBinary(data))
				{
					s.tls_close(TlsAlertDescription::decode_error);
					return;
				}
				handshaker->cipher_suite = shello.cipher_suite;
				handshaker->server_random = shello.random.toBinary();

				s.tls_recvHandshake(std::move(handshaker), TlsHandshake::certificate, [](Socket& s, UniquePtr<SocketTlsHandshaker>&& handshaker, std::string&& data)
				{
					TlsCertificate cert;
					if (!cert.fromBinary(data))
					{
						s.tls_close(TlsAlertDescription::decode_error);
						return;
					}
					if (!handshaker->certchain.fromDer(cert.asn1_certs)
						|| !handshaker->certchain_validator(handshaker->certchain, handshaker->server_name)
						)
					{
						s.tls_close(TlsAlertDescription::bad_certificate);
						return;
					}

					switch (handshaker->cipher_suite)
					{
					default:
						s.enableCryptoClientRecvServerHelloDone(std::move(handshaker));
						break;

					case TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA:
					case TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256:
					case TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA:
						s.tls_recvHandshake(std::move(handshaker), TlsHandshake::server_key_exchange, [](Socket& s, UniquePtr<SocketTlsHandshaker>&& handshaker, std::string&& data)
						{
							TlsServerKeyExchange ske;
							if (!ske.fromBinary(data))
							{
								s.tls_close(TlsAlertDescription::decode_error);
								return;
							}
							if (ske.named_curve != NamedCurves::x25519
								|| ske.point.size() != ec::X25519_KEY_SIZE
								)
							{
								s.tls_close(TlsAlertDescription::handshake_failure);
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

	void Socket::enableCryptoClientRecvServerHelloDone(UniquePtr<SocketTlsHandshaker>&& handshaker)
	{
		tls_recvHandshake(std::move(handshaker), TlsHandshake::server_hello_done, [](Socket& s, UniquePtr<SocketTlsHandshaker>&& handshaker, std::string&& data)
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

				TlsEncryptedPreMasterSecret epms{};
				epms.data = handshaker->certchain.certs.at(0).key.encryptPkcs1(pms).toBinary();
				cke = epms.toBinary();

				handshaker->pre_master_secret = soup::make_unique<Promise<std::string>>(std::move(pms));
			}
			else
			{
				uint8_t my_priv[ec::X25519_KEY_SIZE];
				ec::curve25519_generatePrivate(my_priv);

				uint8_t their_pub[ec::X25519_KEY_SIZE];
				memcpy(their_pub, handshaker->server_x25519_public_key.data(), sizeof(their_pub));

				uint8_t shared_secret[ec::X25519_SHARED_SIZE];
				ec::x25519(shared_secret, my_priv, their_pub);
				handshaker->pre_master_secret = soup::make_unique<Promise<std::string>>(std::string((const char*)shared_secret, sizeof(shared_secret)));

				uint8_t my_pub[ec::X25519_KEY_SIZE];
				ec::curve25519_derivePublic(my_pub, my_priv);

				cke = std::string(1, (char)sizeof(my_pub));
				cke.append((const char*)my_pub, sizeof(my_pub));
			}
			if (s.tls_sendHandshake(handshaker, TlsHandshake::client_key_exchange, std::move(cke))
				&& s.tls_sendRecord(TlsContentType::change_cipher_spec, "\1")
				)
			{
				handshaker->getKeys(s.tls_encrypter_send.mac_key, handshaker->pending_recv_encrypter.mac_key, s.tls_encrypter_send.cipher_key, handshaker->pending_recv_encrypter.cipher_key);
				if (s.tls_sendHandshake(handshaker, TlsHandshake::finished, handshaker->getClientFinishVerifyData()))
				{
					s.tls_recvRecord(TlsContentType::change_cipher_spec, [](Socket& s, std::string&& data, Capture&& cap)
					{
						UniquePtr<SocketTlsHandshaker> handshaker = std::move(cap.get<UniquePtr<SocketTlsHandshaker>>());

						s.tls_encrypter_recv = std::move(handshaker->pending_recv_encrypter);

						handshaker->expected_finished_verify_data = handshaker->getServerFinishVerifyData();

						s.tls_recvHandshake(std::move(handshaker), TlsHandshake::finished, [](Socket& s, UniquePtr<SocketTlsHandshaker>&& handshaker, std::string&& data)
						{
							if (data != handshaker->expected_finished_verify_data)
							{
								s.tls_close(TlsAlertDescription::decrypt_error);
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

	struct CaptureDecryptPreMasterSecret
	{
		SocketTlsHandshaker* handshaker;
		Bigint data;
	};

	void Socket::enableCryptoServer(tls_server_cert_selector_t cert_selector, void(*callback)(Socket&, Capture&&), Capture&& cap, tls_server_on_client_hello_t on_client_hello)
	{
		auto handshaker = make_unique<SocketTlsHandshaker>(
			callback,
			std::move(cap)
		);
		handshaker->cert_selector = cert_selector;
		handshaker->on_client_hello = on_client_hello;
		tls_recvHandshake(std::move(handshaker), TlsHandshake::client_hello, [](Socket& s, UniquePtr<SocketTlsHandshaker>&& handshaker, std::string&& data)
		{
			{
				TlsClientHello hello;
				if (!hello.fromBinary(data))
				{
					s.tls_close(TlsAlertDescription::decode_error);
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
					if (ext.id == TlsExtensionType::server_name)
					{
						TlsClientHelloExtServerName ext_server_name;
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
					s.tls_close(TlsAlertDescription::handshake_failure);
					return;
				}

				handshaker->client_random = hello.random.toBinary();

				if (handshaker->on_client_hello)
				{
					handshaker->on_client_hello(s, std::move(hello));
				}
			}

			{
				TlsServerHello shello{};
				shello.random.time = time::unixSeconds();
				rand.fill(shello.random.random);
				handshaker->server_random = shello.random.toBinary();
				shello.cipher_suite = handshaker->cipher_suite;
				shello.compression_method = 0;
				if (!s.tls_sendHandshake(handshaker, TlsHandshake::server_hello, shello.toBinary()))
				{
					return;
				}
			}

			{
				TlsCertificate tcert;
				tcert.asn1_certs = std::move(handshaker->rsa_data.der_encoded_certchain);
				if (!s.tls_sendHandshake(handshaker, TlsHandshake::certificate, tcert.toBinary()))
				{
					return;
				}
			}

			if (!s.tls_sendHandshake(handshaker, TlsHandshake::server_hello_done, {}))
			{
				return;
			}

			s.tls_recvHandshake(std::move(handshaker), TlsHandshake::client_key_exchange, [](Socket& s, UniquePtr<SocketTlsHandshaker>&& handshaker, std::string&& data)
			{
				if (data.size() <= 2)
				{
					s.tls_close(TlsAlertDescription::decode_error);
					return;
				}
				data.erase(0, 2); // length prefix

				handshaker->pre_master_secret = make_unique<Promise<std::string>>([](Capture&& _cap, PromiseBase*)
				{
					auto& cap = _cap.get<CaptureDecryptPreMasterSecret>();
					return cap.handshaker->rsa_data.private_key.decryptPkcs1(cap.data);
				}, CaptureDecryptPreMasterSecret{
					handshaker.get(),
					Bigint::fromBinary(data)
				});

				s.tls_recvRecord(TlsContentType::change_cipher_spec, [](Socket& s, std::string&& data, Capture&& cap)
				{
					if (!s.tls_sendRecord(TlsContentType::change_cipher_spec, "\1"))
					{
						return;
					}

					UniquePtr<SocketTlsHandshaker> handshaker = std::move(cap.get<UniquePtr<SocketTlsHandshaker>>());

					PromiseBase* p = handshaker->pre_master_secret.get();

					s.awaitPromiseCompletion(p, [](Worker& w, Capture&& cap)
					{
						auto& s = reinterpret_cast<Socket&>(w);
						UniquePtr<SocketTlsHandshaker> handshaker = std::move(cap.get<UniquePtr<SocketTlsHandshaker>>());

						handshaker->getKeys(s.tls_encrypter_recv.mac_key, s.tls_encrypter_send.mac_key, s.tls_encrypter_recv.cipher_key, s.tls_encrypter_send.cipher_key);

						handshaker->expected_finished_verify_data = handshaker->getClientFinishVerifyData();

						s.tls_recvHandshake(std::move(handshaker), TlsHandshake::finished, [](Socket& s, UniquePtr<SocketTlsHandshaker>&& handshaker, std::string&& data)
						{
							if (data != handshaker->expected_finished_verify_data)
							{
								s.tls_close(TlsAlertDescription::decrypt_error);
								return;
							}

							if (s.tls_sendHandshake(handshaker, TlsHandshake::finished, handshaker->getServerFinishVerifyData()))
							{
								handshaker->callback(s, std::move(handshaker->callback_capture));
							}
						});
					}, std::move(handshaker));
				}, std::move(handshaker));
			});
		});
	}

	bool Socket::isEncrypted() const noexcept
	{
		return tls_encrypter_send.isActive();
	}

	bool Socket::send(const std::string& data)
	{
		if (tls_encrypter_send.isActive())
		{
			return tls_sendRecordEncrypted(TlsContentType::application_data, data);
		}
		return transport_send(data);
	}

	struct CaptureSocketRecv
	{
		void(*callback)(Socket&, std::string&&, Capture&&);
		Capture cap;
	};

	void Socket::recv(void(*callback)(Socket&, std::string&&, Capture&&), Capture&& cap)
	{
		CaptureSocketRecv inner_cap{
			callback,
			std::move(cap)
		};
		auto inner_callback = [](Socket& s, std::string&& data, Capture&& _cap)
		{
			auto& cap = _cap.get<CaptureSocketRecv>();
			cap.callback(s, std::move(data), std::move(cap.cap));
		};
		if (tls_encrypter_recv.isActive())
		{
			tls_recvRecord(TlsContentType::application_data, inner_callback, std::move(inner_cap));
		}
		else
		{
			transport_recv(8192, inner_callback, std::move(inner_cap));
		}
	}

	void Socket::close()
	{
		if (tls_encrypter_send.isActive())
		{
			tls_close(TlsAlertDescription::close_notify);
		}
		else
		{
			transport_close();
		}
	}

	bool Socket::tls_sendHandshake(const UniquePtr<SocketTlsHandshaker>& handshaker, TlsHandshakeType_t handshake_type, const std::string& content)
	{
		return tls_sendRecord(TlsContentType::handshake, handshaker->pack(handshake_type, content));
	}

	bool Socket::tls_sendRecord(TlsContentType_t content_type, const std::string& content)
	{
		if (!tls_encrypter_send.isActive())
		{
			TlsRecord record{};
			record.content_type = content_type;
			record.length = content.size();
			auto data = record.toBinary();
			data.append(content);
			return transport_send(data);
		}

		return tls_sendRecordEncrypted(content_type, content);
	}

	bool Socket::tls_sendRecordEncrypted(TlsContentType_t content_type, const std::string& content)
	{
		std::string body = tls_encrypter_send.encrypt(content_type, content);

		TlsRecord record{};
		record.content_type = content_type;
		record.length = body.size();
		auto data = record.toBinary();
		data.append(body);
		return transport_send(data);
	}

	struct CaptureSocketTlsRecvHandshake
	{
		UniquePtr<SocketTlsHandshaker> handshaker;
		TlsHandshakeType_t expected_handshake_type;
		void(*callback)(Socket&, UniquePtr<SocketTlsHandshaker>&&, std::string&&);
		std::string pre;
		bool is_new_bytes = false;
	};

	void Socket::tls_recvHandshake(UniquePtr<SocketTlsHandshaker>&& handshaker, TlsHandshakeType_t expected_handshake_type, void(*callback)(Socket&, UniquePtr<SocketTlsHandshaker>&&, std::string&&), std::string&& pre)
	{
		CaptureSocketTlsRecvHandshake cap{
			std::move(handshaker),
			expected_handshake_type,
			callback,
			std::move(pre)
		};

		auto record_callback = [](Socket& s, TlsContentType_t content_type, std::string&& data, Capture&& _cap)
		{
			if (content_type != TlsContentType::handshake)
			{
				s.tls_close(TlsAlertDescription::unexpected_message);
				return;
			}

			auto& cap = _cap.get<CaptureSocketTlsRecvHandshake>();

			if (cap.is_new_bytes)
			{
				cap.handshaker->layer_bytes.append(data);
			}

			if (!cap.pre.empty())
			{
				data.insert(0, cap.pre);
			}

			TlsHandshake hs;
			if (!hs.fromBinary(data))
			{
				s.tls_close(TlsAlertDescription::decode_error);
				return;
			}

			if (hs.handshake_type != cap.expected_handshake_type)
			{
				s.tls_close(TlsAlertDescription::unexpected_message);
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
				s.tls_record_buf = data.substr(hs.length);
				data.erase(hs.length);
			}

			cap.callback(s, std::move(cap.handshaker), std::move(data));
		};

		if (!tls_record_buf.empty())
		{
			std::string data = std::move(tls_record_buf);
			tls_record_buf.clear();
			record_callback(*this, TlsContentType::handshake, std::move(data), std::move(cap));
			return;
		}

		cap.is_new_bytes = true;
		tls_recvRecord(record_callback, std::move(cap));
	}

	struct CaptureSocketTlsRecvRecordExpect
	{
		TlsContentType_t expected_content_type;
		void(*callback)(Socket&, std::string&&, Capture&&);
		Capture cap;
	};

	void Socket::tls_recvRecord(TlsContentType_t expected_content_type, void(*callback)(Socket&, std::string&&, Capture&&), Capture&& cap)
	{
		tls_recvRecord([](Socket& s, TlsContentType_t content_type, std::string&& data, Capture&& _cap)
		{
			auto& cap = _cap.get<CaptureSocketTlsRecvRecordExpect>();
			if (content_type != cap.expected_content_type)
			{
				s.tls_close(TlsAlertDescription::unexpected_message);
				return;
			}
			cap.callback(s, std::move(data), std::move(cap.cap));
		}, CaptureSocketTlsRecvRecordExpect{
			expected_content_type,
			callback,
			std::move(cap)
		});
	}

	struct CaptureSocketTlsRecvRecord1
	{
		void(*callback)(Socket&, TlsContentType_t, std::string&&, Capture&&);
		Capture cap;
	};

	struct CaptureSocketTlsRecvRecord2
	{
		CaptureSocketTlsRecvRecord1 prev;
		TlsContentType_t content_type;
	};

	void Socket::tls_recvRecord(void(*callback)(Socket&, TlsContentType_t, std::string&&, Capture&&), Capture&& cap)
	{
		if (!tls_record_buf.empty())
		{
			std::string data = std::move(tls_record_buf);
			tls_record_buf.clear();
			callback(*this, TlsContentType::handshake, std::move(data), std::move(cap));
			return;
		}
		transport_recvExact(5, [](Socket& s, std::string&& data, Capture&& cap)
		{
			TlsRecord record{};
			if (!record.fromBinary(data))
			{
				s.tls_close(TlsAlertDescription::decode_error);
				return;
			}
			s.transport_recvExact(record.length, [](Socket& s, std::string&& data, Capture&& _cap)
			{
				auto& cap = _cap.get<CaptureSocketTlsRecvRecord2>();
				if (s.tls_encrypter_recv.isActive())
				{
					constexpr auto cipher_bytes = 16;
					const auto mac_length = s.tls_encrypter_recv.mac_key.size();

					if ((data.size() % cipher_bytes) != 0
						|| data.size() < (cipher_bytes + mac_length)
						)
					{
						s.tls_close(TlsAlertDescription::bad_record_mac);
						return;
					}

					auto iv = data.substr(0, cipher_bytes);
					data.erase(0, cipher_bytes);
					data = aes::decryptCBC(data, s.tls_encrypter_recv.cipher_key, iv);

					std::string mac{};

					bool pad_mismatch = false;
					uint8_t pad_len = data.back();
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
						s.tls_close(TlsAlertDescription::bad_record_mac);
						return;
					}
				}
				cap.prev.callback(s, cap.content_type, std::move(data), std::move(cap.prev.cap));
			}, CaptureSocketTlsRecvRecord2{
				std::move(cap.get<CaptureSocketTlsRecvRecord1>()),
				record.content_type
			});
		}, CaptureSocketTlsRecvRecord1{
			callback,
			std::move(cap)
		});
	}

	void Socket::tls_close(TlsAlertDescription_t desc)
	{
		if (hasConnection())
		{
			{
				std::string bin(1, '\2'); // fatal
				bin.push_back((char)desc); static_assert(sizeof(TlsAlertDescription_t) == sizeof(char));
				tls_sendRecord(TlsContentType::alert, bin);
			}

			tls_encrypter_send.reset();
			tls_encrypter_recv.reset();

			transport_close();
		}
	}

	bool Socket::transport_hasData() const
	{
		char buf;
		return ::recv(fd, &buf, 1, MSG_PEEK) == 1;
	}

	bool Socket::transport_send(const std::string& data) const noexcept
	{
		return transport_send(data.data(), data.size());
	}

	bool Socket::transport_send(const void* data, int size) const noexcept
	{
		return ::send(fd, (const char*)data, size, 0) == size;
	}

	std::string Socket::transport_recvCommon(int max_bytes)
	{
		std::string buf(max_bytes, '\0');
		auto res = ::recv(fd, buf.data(), max_bytes, 0);
		if (res > 0)
		{
			buf.resize(res);
			return buf;
		}
#if SOUP_LINUX
		/*else*/ if (res == 0
			|| (/*res == -1 &&*/
//#if SOUP_WINDOWS
//				WSAGetLastError() != WSAEWOULDBLOCK
//#else
				errno != EWOULDBLOCK && errno != EAGAIN
//#endif
				)
			)
		{
			close();
		}
#endif
		return {};
	}

	struct CaptureSocketTransportRecv
	{
		int bytes;
		Socket::transport_recv_callback_t callback;
		Capture cap;
	};

	void Socket::transport_recv(int max_bytes, transport_recv_callback_t callback, Capture&& cap)
	{
		if (canRecurse())
		{
			if (auto buf = transport_recvCommon(max_bytes); !buf.empty())
			{
				callback(*this, std::move(buf), std::move(cap));
				return;
			}
		}
		holdup_type = SOCKET;
		holdup_callback.set([](Worker& w, Capture&& _cap)
		{
			auto& cap = _cap.get<CaptureSocketTransportRecv>();
			reinterpret_cast<Socket&>(w).transport_recv(cap.bytes, cap.callback, std::move(cap.cap));
		}, CaptureSocketTransportRecv{ max_bytes, callback, std::move(cap) });
	}

	struct CaptureSocketTransportRecvExact : public CaptureSocketTransportRecv
	{
		std::string buf;

		CaptureSocketTransportRecvExact(int bytes, Socket::transport_recv_callback_t callback, Capture&& cap, std::string&& buf)
			: CaptureSocketTransportRecv{ bytes, callback, std::move(cap) }, buf(std::move(buf))
		{
		}
	};

	void Socket::transport_recvExact(int bytes, transport_recv_callback_t callback, Capture&& cap, std::string&& pre)
	{
		if (canRecurse())
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
			if (remote_closed)
			{
				return;
			}
		}
		holdup_type = SOCKET;
		holdup_callback.set([](Worker& w, Capture&& _cap)
		{
			auto& cap = _cap.get<CaptureSocketTransportRecvExact>();
			reinterpret_cast<Socket&>(w).transport_recvExact(cap.bytes, cap.callback, std::move(cap.cap), std::move(cap.buf));
		}, CaptureSocketTransportRecvExact(bytes, callback, std::move(cap), std::move(pre)));
	}

	void Socket::transport_close() noexcept
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

#endif
