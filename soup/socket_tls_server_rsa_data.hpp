#pragma once

#include <string>
#include <vector>

#include "rsa.hpp"

namespace soup
{
	struct socket_tls_server_rsa_data
	{
		std::vector<std::string> der_encoded_certchain{};
		rsa::key_private private_key{};
	};
}
