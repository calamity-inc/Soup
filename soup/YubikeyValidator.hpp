#pragma once

#include "base.hpp"
#if !SOUP_WASM

#include <string>

namespace soup
{
	class YubikeyValidator
	{
	public:
		struct Result
		{
			std::string device_id;

			[[nodiscard]] bool isValid() const noexcept
			{
				return !device_id.empty();
			}
		};

		// You can sign up for an API key at https://upgrade.yubico.com/getapikey/
		YubikeyValidator(const std::string& id, const std::string& secret);

		[[nodiscard]] Result validate(const std::string& otp) const; // blocking

	protected:
		std::string base_path;
		std::string secret;
	};
}

#endif
