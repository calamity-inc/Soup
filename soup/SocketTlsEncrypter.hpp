#pragma once

#include <string>
#include <vector>

#include "base.hpp"
#include "type.hpp"

// AES-GCM on 32-bit build seems to result in remote closing with bad_record_mac
#if SOUP_BITS == 32
#define SOUP_TLS_SUPPORTS_AEAD false
#else
#define SOUP_TLS_SUPPORTS_AEAD true
#endif

namespace soup
{
	struct SocketTlsEncrypter
	{
		uint64_t seq_num = 0;
		std::vector<uint8_t> cipher_key;
		std::string mac_key;
		std::vector<uint8_t> implicit_iv;

		[[nodiscard]] constexpr bool isActive() const noexcept
		{
			return !cipher_key.empty();
		}

		[[nodiscard]] constexpr bool isAead() const noexcept
		{
#if SOUP_TLS_SUPPORTS_AEAD
			return !implicit_iv.empty();
#else
			return false;
#endif
		}

		[[nodiscard]] size_t getMacLength() const noexcept;
		[[nodiscard]] std::string calculateMacBytes(TlsContentType_t content_type, const std::string& content);
		[[nodiscard]] std::string calculateMac(TlsContentType_t content_type, const std::string& content);

		[[nodiscard]] std::vector<uint8_t> encrypt(TlsContentType_t content_type, const std::string& content);

		void reset() noexcept;
	};
}
