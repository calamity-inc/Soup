#include "MacAddr.hpp"

#include "string.hpp"

namespace soup
{
	MacAddr MacAddr::DUMMY = MacAddr(0x00, 0x1A, 0x7D, 0xDA, 0x71, 0x15);

	std::string MacAddr::toString(char sep) const
	{
		std::string str;
		str.reserve((6 * 2) + 5);
		str.append(string::lpad(string::hex(data[0]), 2, '0'));
		str.push_back(sep);
		str.append(string::lpad(string::hex(data[1]), 2, '0'));
		str.push_back(sep);
		str.append(string::lpad(string::hex(data[2]), 2, '0'));
		str.push_back(sep);
		str.append(string::lpad(string::hex(data[3]), 2, '0'));
		str.push_back(sep);
		str.append(string::lpad(string::hex(data[4]), 2, '0'));
		str.push_back(sep);
		str.append(string::lpad(string::hex(data[5]), 2, '0'));
		return str;
	}
}
