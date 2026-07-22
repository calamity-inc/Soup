#pragma once

#include "Packet.hpp"

NAMESPACE_SOUP
{
	// Yet another Content-Addressed Archive (CAR) format :)
	SOUP_PACKET(cadArchive)
	{
		uint8_t hash_bytes;
		std::string hash_function; // informational. can definitely be used for integrity checking, but not a hard requirement atm.
		std::string root_hash;
		std::unordered_map<std::string, std::string> objects;

		SOUP_PACKET_IO(s)
		{
			// Shitty magic but it hopefully helps distinguish from other formats named CAR
			char magic[16] = { 'c', 'a', 'l', 'a', 'm', 'i', 't', 'y', '-', 'i', 'n', 'c', '/', 'C', 'A', 'R' };
			s.str(sizeof(magic), magic);
			SOUP_IF_ISREAD
			{
				const char expected_magic[16] = { 'c', 'a', 'l', 'a', 'm', 'i', 't', 'y', '-', 'i', 'n', 'c', '/', 'C', 'A', 'R' };
				SOUP_RETHROW_FALSE(memcmp(magic, expected_magic, sizeof(magic)) == 0);
			}

			uint8_t version = 0;
			SOUP_RETHROW_FALSE(s.u8(version));
			SOUP_IF_ISREAD
			{
				SOUP_RETHROW_FALSE(version == 0);
			}

			uint8_t flags = 0;
			SOUP_IF_ISREAD {} SOUP_ELSEIF_ISWRITE
			{
				if (!root_hash.empty())
				{
					flags |= 1;
				}
			}
			s.u8(flags);

			SOUP_RETHROW_FALSE(s.u8(hash_bytes) && hash_bytes != 0);
			SOUP_RETHROW_FALSE(s.template str_lp<u8_t>(hash_function));

			if (flags & 1)
			{
				SOUP_RETHROW_FALSE(s.str(hash_bytes, root_hash));
			}

			SOUP_IF_ISREAD
			{
				uint64_t num_objects;
				SOUP_RETHROW_FALSE(s.u64_dyn_bp(num_objects));
				while (num_objects--)
				{
					std::string hash;
					SOUP_RETHROW_FALSE(s.str(hash_bytes, hash));
					SOUP_RETHROW_FALSE(s.str_lp_u64_dyn_bp(objects.emplace(std::move(hash), std::string{}).first->second));
				}
			}
			else
			{
				uint64_t num_objects = objects.size();
				SOUP_RETHROW_FALSE(s.u64_dyn_bp(num_objects));
				for (auto& object : objects)
				{
					SOUP_RETHROW_FALSE(s.str(hash_bytes, object.first));
					SOUP_RETHROW_FALSE(s.str_lp_u64_dyn_bp(object.second));
				}
			}

			return true;
		}
	};
}
