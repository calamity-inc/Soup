#pragma once

#include <cstdint>
#include <cstring> // memcpy
#include <string>

namespace soup
{
	// Adapted from https://gist.github.com/tomykaira/f0fd86b6c73063283afe550bc5d77594

	struct base64
	{
		[[nodiscard]] static std::string encode(const std::string& data, const bool pad = true) noexcept;
		[[nodiscard]] static std::string encode(const char* const data, const size_t size, const bool pad = true) noexcept;

		template <typename T>
		[[nodiscard]] static std::string encode(const T& data, const bool pad = true) noexcept
		{
			return encode(&data, pad);
		}

		template <typename T>
		[[nodiscard]] static std::string encode(const T* const data, const bool pad = true) noexcept
		{
			return encode(static_cast<const char*>(&data), sizeof(T), pad);
		}

		[[nodiscard]] static std::string decode(std::string enc);

		template <typename T>
		static bool decode(T& out, std::string enc) noexcept
		{
			std::string tmp = decode(std::move(enc));
			if (tmp.size() != sizeof(T))
			{
				return false;
			}
			memcpy(&out, tmp.data(), sizeof(T));
			return true;
		}
	};
}
