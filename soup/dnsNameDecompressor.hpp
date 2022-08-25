#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "ioSizeMeasurer.hpp"

namespace soup
{
	struct dnsNameDecompressor
	{
		uint16_t position = 12; // Byte-length of dnsHeader
		std::unordered_map<uint16_t, std::vector<std::string>> map;

		template <typename Packet>
		const std::vector<std::string>& getString(Packet& p)
		{
			const std::vector<std::string>* nameptr;
			if (p.name.isPointer())
			{
				nameptr = &map.at(p.name.ptr);
			}
			else
			{
				nameptr = &p.name.name;
			}

			map.emplace(position, *nameptr);

			ioSizeMeasurer wsm;
			p.write(wsm);
			position += wsm.size;

			return *nameptr;
		}
	};
}
