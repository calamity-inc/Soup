#pragma once

#include "Packet.hpp"

namespace soup
{
	SOUP_PACKET(dnsName)
	{
		std::vector<std::string> name;
		u16 ptr;

		[[nodiscard]] bool isPointer() const noexcept
		{
			return name.empty();
		}

		SOUP_PACKET_IO(s)
		{
			SOUP_IF_ISREAD
			{
				name.clear();
				while (true)
				{
					uint8_t len;
					if (!s.u8(len))
					{
						return false;
					}
					if ((len >> 6) & 0b11)
					{
						if (!name.empty())
						{
							return false;
						}
						ptr = (len & 0b111111);
						ptr <<= 8;
						if (!s.u8(len))
						{
							return false;
						}
						ptr |= len;
						break;
					}
					std::string entry;
					if (!s.str(len, entry))
					{
						return false;
					}
					if (entry.empty())
					{
						break;
					}
					name.emplace_back(std::move(entry));
				}
			}
			SOUP_ELSEIF_ISWRITE
			{
				if (isPointer())
				{
					uint16_t data = ptr;
					data |= 0xC000;
					return s.u16(data);
				}
				else
				{
					for (const auto& entry : name)
					{
						if (!s.str_lp_u8(entry))
						{
							return false;
						}
					}
					uint8_t root = 0;
					return s.u8(root);
				}
			}
			return true;
		}
	};
}
