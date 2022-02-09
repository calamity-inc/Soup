#pragma once

#include <string>

namespace soup
{
	template <typename T>
	struct crypto_hash_algo
	{
		[[nodiscard]] static std::string hashWithId(const std::string& msg)
		{
			auto hash_bin = T::hash(msg);
			prependId(hash_bin);
			return hash_bin;
		}

		static bool prependId(std::string& hash_bin)
		{
			if (hash_bin.length() != T::DIGEST_BYTES)
			{
				if (hash_bin.length() > T::DIGEST_BYTES)
				{
					return false;
				}
				hash_bin.insert(0, T::DIGEST_BYTES - hash_bin.length(), '\0');
			}
			hash_bin.insert(0, T::ID, sizeof(T::ID)); // As per https://datatracker.ietf.org/doc/html/rfc3447#page-43
			return true;
		}
	};
}
