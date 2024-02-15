#pragma once

#include "Packet.hpp"

namespace soup
{
	SOUP_PACKET(TlsServerECDHParams)
	{
		u8 curve_type;
		u16 named_curve;
		std::string point;

		SOUP_PACKET_IO(s)
		{
			return s.u8(curve_type)
				&& curve_type == 3
				&& s.u16(named_curve)
				&& s.template str_lp<u8_t>(point)
				;
		}
	};
	
	SOUP_PACKET(TlsServerKeyExchange)
	{
		TlsServerECDHParams params;

		u8 signature_hash; // HashAlgorithm - https://www.iana.org/assignments/tls-parameters/tls-parameters.xhtml#tls-parameters-18
		u8 signature_algo; // SignatureAlgorithm - https://www.iana.org/assignments/tls-parameters/tls-parameters.xhtml#tls-parameters-16
		std::string signature;

		SOUP_PACKET_IO(s)
		{
			return params.io(s)
				&& s.u8(signature_hash)
				&& s.u8(signature_algo)
				&& s.template str_lp<u16_t>(signature)
				;
		}
	};
}
