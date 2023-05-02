#pragma once

#include "scoped_enum.hpp"

namespace soup
{
	SCOPED_ENUM(TlsExtensionType, uint16_t,
		server_name = 0,
		elliptic_curves = 10,
		ec_point_formats = 11,
		signature_algorithms = 13,
	);
}
