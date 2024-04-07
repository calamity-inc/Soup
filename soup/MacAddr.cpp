#include "MacAddr.hpp"

#include "string.hpp"

NAMESPACE_SOUP
{
	MacAddr MacAddr::SOUP_FIRST = MacAddr(0x8C, 0x1F, 0x64, 0xB9, 0x85, 0x00);
	MacAddr MacAddr::SOUP_LAST = MacAddr(0x8C, 0x1F, 0x64, 0xB9, 0x85, 0xFF);

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
