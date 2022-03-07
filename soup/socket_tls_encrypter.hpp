#pragma once

#include <string>

#include "type.hpp"

namespace soup
{
	struct socket_tls_encrypter
	{
		uint64_t seq_num = 0;
		std::string cipher_key;
		std::string mac_key;

		[[nodiscard]] constexpr bool isActive() const noexcept
		{
			return !cipher_key.empty();
		}

		[[nodiscard]] size_t getMacLength() const noexcept;
		[[nodiscard]] std::string calculateMac(tls_content_type_t content_type, const std::string& content);

		[[nodiscard]] std::string encrypt(tls_content_type_t content_type, std::string content);

		void reset() noexcept;
	};
}
