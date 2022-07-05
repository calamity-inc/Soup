#include "hotp.hpp"

#include "intutil.hpp"
#include "sha1.hpp"
#include "StringWriter.hpp"

namespace soup
{
	int hotp(const std::string& secret, uint64_t counter, uint8_t digits)
	{
		StringWriter w(false);
		w.u64(counter);

		std::string mac = sha1::hmac(w.str, secret);

		auto offset = ((uint8_t)mac.at(19) & 0xf);
		int bin_code = ((uint8_t)mac.at(offset) & 0x7f) << 24
			| ((uint8_t)mac.at(offset + 1) & 0xff) << 16
			| ((uint8_t)mac.at(offset + 2) & 0xff) << 8
			| ((uint8_t)mac.at(offset + 3) & 0xff);

		bin_code %= intutil::pow<int>(10, digits);
		return bin_code;
	}
}

