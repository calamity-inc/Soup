#pragma once

#include "fwd.hpp"

#include <string>

namespace soup
{
	struct acme
	{
		[[nodiscard]] static std::string getNewAcctPayload(const rsaKeypair& kp, const std::string& nonce, std::string email);
	};
}
