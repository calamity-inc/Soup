#pragma once

#include "packet.hpp"

namespace soup
{
	SOUP_PACKET(tls_hello_extension)
	{
		uint16_t id;
		std::string data;

		SOUP_PACKET_IO(s)
		{
			return s.u16(id)
				&& s.str_lp_u16(data)
				;
		}
	};

	SOUP_PACKET(tls_hello_extensions)
	{
		std::vector<tls_hello_extension> extensions{};

		SOUP_PACKET_IO(s)
		{
			if (s.isRead())
			{
				extensions.clear();
				if (s.hasMore())
				{
					uint16_t extension_bytes;
					if (!s.u16(extension_bytes))
					{
						return false;
					}
					while (extension_bytes >= 4)
					{
						tls_hello_extension ext;
						if (!ext.io(s))
						{
							return false;
						}
						extension_bytes -= (4 + ext.data.size());
						extensions.emplace_back(std::move(ext));
					}
				}
			}
			else if (s.isWrite())
			{
				if (!extensions.empty())
				{
					std::string ext_data{};
					for (auto& ext : extensions)
					{
						ext_data.append(ext.toBinary());
					}
					s.str_lp_u16(ext_data);
				}
			}
			return true;
		}

		template <typename T>
		void add(uint16_t ext_id, packet<T>& ext_packet)
		{
			return add(ext_id, ext_packet.toBinary());
		}

		void add(uint16_t ext_id, std::string&& ext_data) noexcept
		{
			tls_hello_extension ext;
			ext.id = ext_id;
			ext.data = std::move(ext_data);
			extensions.emplace_back(std::move(ext));
		}
	};
}
