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

		static std::string hmac(const std::string& msg, std::string key)
		{
			if (key.length() > T::DIGEST_BYTES)
			{
				key = T::hash(key);
			}

			std::string inner = key;
			std::string outer = key;

			for (auto i = 0; i != key.length(); ++i)
			{
				inner[i] ^= 0x36;
				outer[i] ^= 0x5c;
			}

			if (auto diff = T::BLOCK_BYTES - key.length(); diff != 0)
			{
				inner.append(diff, '\x36');
				outer.append(diff, '\x5c');
			}

			inner.append(msg);
			outer.append(T::hash(std::move(inner)));
			return T::hash(std::move(outer));
		}
	};
}
