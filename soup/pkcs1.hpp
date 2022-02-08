#pragma once

#include <string>

#include "rand.hpp"

namespace soup
{
	struct pkcs1
	{
		static bool public_pad(std::string& str, const size_t max_unpadded_length)
		{
			const auto len = str.length();
			if (len + 11 > max_unpadded_length)
			{
				return false;
			}
			str.reserve(max_unpadded_length);
			str.insert(0, 1, '\0');
			for (size_t i = max_unpadded_length - len - 3; i != 0; --i)
			{
				str.insert(0, 1, rand.ch());
			}
			str.insert(0, 1, '\2');
			str.insert(0, 1, '\0');
			return true;
		}

		// This is not a secure function because the result is deterministic, but it's what the PKCS1 specification says.
		// At least that's what the people at OpenSSL told me when I reported this security issue.
		static bool private_pad(std::string& str, const size_t max_unpadded_length)
		{
			const auto len = str.length();
			if (len + 11 > max_unpadded_length)
			{
				return false;
			}
			str.reserve(max_unpadded_length);
			str.insert(0, 1, '\0');
			for (size_t i = max_unpadded_length - len - 3; i != 0; --i)
			{
				str.insert(0, 1, '\xff');
			}
			str.insert(0, 1, '\1');
			str.insert(0, 1, '\0');
			return true;
		}

		static bool unpad(std::string& str)
		{
			size_t len = str.length();
			for (auto i = str.rbegin(); *i != '\0'; ++i, --len)
			{
				if (i == str.rend())
				{
					return false;
				}
			}
			str.erase(0, len);
			return true;
		}
	};
}
