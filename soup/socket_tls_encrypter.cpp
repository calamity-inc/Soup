#include "socket_tls_encrypter.hpp"

#include "aes.hpp"
#include "rand.hpp"
#include "sha1.hpp"
#include "sha256.hpp"
#include "tls_mac.hpp"

namespace soup
{
	size_t socket_tls_encrypter::getMacLength() const noexcept
	{
		return mac_key.size();
	}

	std::string socket_tls_encrypter::calculateMac(tls_content_type_t content_type, const std::string& content)
	{
		tls_mac mac{};
		mac.seq_num = seq_num++;
		mac.record.content_type = content_type;
		mac.record.length = content.size();
		auto msg = mac.toBinary();
		msg.append(content);

		if (mac_key.size() == 20)
		{
			return sha1::hmac(msg, mac_key);
		}
		//else if (mac_key.size() == 32)
		{
			return sha256::hmac(msg, mac_key);
		}
	}

	std::string socket_tls_encrypter::encrypt(tls_content_type_t content_type, std::string content)
	{
		content.append(calculateMac(content_type, content));

		constexpr auto cipher_bytes = 16;

		auto in_len = ((((content.size() + 1) / cipher_bytes) + 1) * cipher_bytes);
		char pad_len = (in_len - content.size());
		content.append((size_t)pad_len, (pad_len - 1));

		auto iv = rand.vec_u8(cipher_bytes);
		std::vector<unsigned char> in(content.begin(), content.end());
		std::vector<unsigned char> key(cipher_key.begin(), cipher_key.end());
		auto out = aes::encryptCBC(in, key, iv);

		std::string res(iv.begin(), iv.end());
		res.append(out.begin(), out.end());
		return res;
	}

	void socket_tls_encrypter::reset() noexcept
	{
		seq_num = 0;
		cipher_key.clear();
		mac_key.clear();
	}
}
