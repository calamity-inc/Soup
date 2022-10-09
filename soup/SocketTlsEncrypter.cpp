#include "SocketTlsEncrypter.hpp"

#include "aes.hpp"
#include "rand.hpp"
#include "sha1.hpp"
#include "sha256.hpp"
#include "TlsMac.hpp"

namespace soup
{
	size_t SocketTlsEncrypter::getMacLength() const noexcept
	{
		return mac_key.size();
	}

	std::string SocketTlsEncrypter::calculateMac(TlsContentType_t content_type, const std::string& content)
	{
		TlsMac mac{};
		mac.seq_num = seq_num++;
		mac.record.content_type = content_type;
		mac.record.length = content.size();
		auto msg = mac.toBinaryString();
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

	std::string SocketTlsEncrypter::encrypt(TlsContentType_t content_type, const std::string& content)
	{
		constexpr auto cipher_bytes = 16;

		auto mac = calculateMac(content_type, content);
		auto cont_with_mac_size = (content.size() + mac.size());
		auto aligned_in_len = ((((cont_with_mac_size + 1) / cipher_bytes) + 1) * cipher_bytes);
		char pad_len = (aligned_in_len - cont_with_mac_size);

		std::vector<uint8_t> in{};
		in.reserve(content.size() + mac.size() + pad_len);
		in.insert(in.end(), content.begin(), content.end());
		in.insert(in.end(), mac.begin(), mac.end());
		in.insert(in.end(), (size_t)pad_len, (pad_len - 1));

		auto iv = rand.vec_u8(cipher_bytes);
		std::vector<uint8_t> key(cipher_key.begin(), cipher_key.end());
		auto out = aes::encryptCBC(in, key, iv);

		std::string res(iv.begin(), iv.end());
		res.append(out.begin(), out.end());
		return res;
	}

	void SocketTlsEncrypter::reset() noexcept
	{
		seq_num = 0;
		cipher_key.clear();
		mac_key.clear();
	}
}
