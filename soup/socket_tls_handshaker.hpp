#pragma once

#include "fwd.hpp"

#include "capture.hpp"
#include "certchain.hpp"
#include "promise.hpp"
#include "socket_tls_encrypter.hpp"
#include "socket_tls_server_rsa_data.hpp"
#include "tls_cipher_suite.hpp"
#include "x509_certificate.hpp"

namespace soup
{
	class socket_tls_handshaker
	{
	public:
		void(*callback)(socket&, capture&&);
		capture callback_capture;

		tls_cipher_suite_t cipher_suite = TLS_RSA_WITH_AES_128_CBC_SHA;
		std::string layer_bytes{};
		std::string client_random{};
		std::string server_random{};
		std::unique_ptr<promise<std::string>> pre_master_secret{};
		std::string master_secret{};
		std::string expected_finished_verify_data{};

		// client
		bool(*certchain_validator)(const certchain&);
		certchain m_certchain{};
		std::string server_x25519_public_key{};
		socket_tls_encrypter pending_recv_encrypter;

		// server
		void(*cert_selector)(socket_tls_server_rsa_data& out, const std::string& server_name);
		socket_tls_server_rsa_data rsa_data{};

		explicit socket_tls_handshaker(void(*callback)(socket&, capture&&), capture&& callback_capture);

		[[nodiscard]] std::string pack(tls_handshake_type_t handshake_type, const std::string& content);
		[[nodiscard]] int unpack(tls_handshake_type_t expected_handshake_type, std::string& content);

		[[nodiscard]] std::string getMasterSecret();
		void getKeys(std::string& client_write_mac, std::string& server_write_mac, std::string& client_write_key, std::string& server_write_key);

		[[nodiscard]] std::string getClientFinishVerifyData();
		[[nodiscard]] std::string getServerFinishVerifyData();
	private:
		[[nodiscard]] std::string getFinishVerifyData(const std::string& label);
	};
}
