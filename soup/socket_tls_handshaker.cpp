#include "socket_tls_handshaker.hpp"

#include "obfus_string.hpp"
#include "sha256.hpp"
#include "tls_handshake.hpp"

namespace soup
{
	socket_tls_handshaker::socket_tls_handshaker(void(*callback)(socket&, capture&&), capture&& callback_capture)
		: callback(callback), callback_capture(std::move(callback_capture))
	{
	}

	std::string socket_tls_handshaker::pack(tls_handshake_type_t handshake_type, const std::string& content)
	{
		tls_handshake hs{};
		hs.handshake_type = handshake_type;
		hs.length = content.size();
		auto data = hs.toBinary();
		data.append(content);

		layer_bytes.append(data);
		return data;
	}

	int socket_tls_handshaker::unpack(tls_handshake_type_t expected_handshake_type, std::string& content)
	{
		tls_handshake hs;
		if (hs.fromBinary(content)
			&& hs.handshake_type == expected_handshake_type
			)
		{
			layer_bytes.append(content);
			content = content.substr(4);
			return hs.length;
		}
		return 0;
	}

	std::string socket_tls_handshaker::getMasterSecret()
	{
		if (pre_master_secret)
		{
			master_secret = sha256::tls_prf(
				obfus_string("master secret"),
				48,
				std::move(pre_master_secret->getResult()),
				std::string(client_random).append(server_random)
			);
			pre_master_secret.reset();
		}
		return master_secret;
	}

	void socket_tls_handshaker::getKeys(std::string& client_write_mac, std::string& server_write_mac, std::string& client_write_key, std::string& server_write_key)
	{
		auto mac_key_length = 20; // SHA1 = 20, SHA256 = 32
		switch (cipher_suite)
		{
		case TLS_RSA_WITH_AES_128_CBC_SHA256:
		case TLS_RSA_WITH_AES_256_CBC_SHA256:
		case TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256:
			mac_key_length = 32;
			break;
		}

		auto enc_key_length = 16; // AES128 = 16, AES256 = 32
		switch (cipher_suite)
		{
		case TLS_RSA_WITH_AES_256_CBC_SHA:
		case TLS_RSA_WITH_AES_256_CBC_SHA256:
		case TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA:
			enc_key_length = 32;
			break;
		}

		auto key_block = sha256::tls_prf(
			obfus_string("key expansion"),
			(mac_key_length * 2) + (enc_key_length * 2),
			getMasterSecret(),
			std::string(server_random).append(client_random)
		);

		client_write_mac = key_block.substr(0, mac_key_length);
		server_write_mac = key_block.substr(mac_key_length, mac_key_length);
		client_write_key = key_block.substr(mac_key_length * 2, enc_key_length);
		server_write_key = key_block.substr((mac_key_length * 2) + enc_key_length);
	}

	std::string socket_tls_handshaker::getClientFinishVerifyData()
	{
		return getFinishVerifyData(obfus_string("client finished"));
	}

	std::string socket_tls_handshaker::getServerFinishVerifyData()
	{
		return getFinishVerifyData(obfus_string("server finished"));
	}

	std::string socket_tls_handshaker::getFinishVerifyData(const std::string& label)
	{
		return sha256::tls_prf(label, 12, getMasterSecret(), sha256::hash(layer_bytes));
	}
}
