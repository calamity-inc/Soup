#pragma once

#include "ZipFileCommon.hpp"

NAMESPACE_SOUP
{
	SOUP_PACKET(ZipLocalFileHeader)
	{
		ZipFileCommon common;
		std::string name;
		std::string extra;

		SOUP_PACKET_IO(s)
		{
			if (!common.io(s))
			{
				return false;
			}
			auto name_length = (u16)name.size();
			auto extra_length = (u16)extra.size();
			return s.u16(name_length)
				&& s.u16(extra_length)
				&& s.str(name_length, name)
				&& s.str(extra_length, extra)
				;
		}
	};
}
