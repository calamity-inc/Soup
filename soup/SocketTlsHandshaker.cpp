#include "SocketTlsHandshaker.hpp"

#if !SOUP_WASM

#include "ObfusString.hpp"
#include "sha256.hpp"
#include "TlsHandshake.hpp"

namespace soup
{
	SocketTlsHandshaker::SocketTlsHandshaker(void(*callback)(Socket&, Capture&&), Capture&& callback_capture)
		: callback(callback), callback_capture(std::move(callback_capture))
	{
	}

	std::string SocketTlsHandshaker::pack(TlsHandshakeType_t handshake_type, const std::string& content)
	{
		TlsHandshake hs{};
		hs.handshake_type = handshake_type;
		hs.length = content.size();
		auto data = hs.toBinaryString();
		data.append(content);

		layer_bytes.append(data);
		return data;
	}

	int SocketTlsHandshaker::unpack(TlsHandshakeType_t expected_handshake_type, std::string& content)
	{
		TlsHandshake hs;
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

	std::string SocketTlsHandshaker::getMasterSecret()
	{
		if (pre_master_secret)
		{
			master_secret = sha256::tls_prf(
				ObfusString("master secret"),
				48,
				std::move(pre_master_secret->getResult()),
				std::string(client_random).append(server_random)
			);
			pre_master_secret.reset();
		}
		return master_secret;
	}

	void SocketTlsHandshaker::getKeys(std::string& client_write_mac, std::string& server_write_mac, std::vector<uint8_t>& client_write_key, std::vector<uint8_t>& server_write_key)
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
			ObfusString("key expansion"),
			(mac_key_length * 2) + (enc_key_length * 2),
			getMasterSecret(),
			std::string(server_random).append(client_random)
		);

		client_write_mac = key_block.substr(0, mac_key_length);
		server_write_mac = key_block.substr(mac_key_length, mac_key_length);

		auto client_write_key_str = key_block.substr(mac_key_length * 2, enc_key_length);
		auto server_write_key_str = key_block.substr((mac_key_length * 2) + enc_key_length);

		client_write_key = std::vector<uint8_t>(client_write_key_str.begin(), client_write_key_str.end());
		server_write_key = std::vector<uint8_t>(server_write_key_str.begin(), server_write_key_str.end());
	}

	std::string SocketTlsHandshaker::getClientFinishVerifyData()
	{
		return getFinishVerifyData(ObfusString("client finished"));
	}

	std::string SocketTlsHandshaker::getServerFinishVerifyData()
	{
		return getFinishVerifyData(ObfusString("server finished"));
	}

	std::string SocketTlsHandshaker::getFinishVerifyData(const std::string& label)
	{
		return sha256::tls_prf(label, 12, getMasterSecret(), sha256::hash(layer_bytes));
	}
}
#endif
