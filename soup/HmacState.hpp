#pragma once

#include "base.hpp"

#include <cstring> // memset, memcpy

NAMESPACE_SOUP
{
	template <typename T>
	struct HmacState
	{
		typename T::State inner;
		typename T::State outer;

		HmacState(const std::string& key) noexcept
			: HmacState(key.data(), key.size())
		{
		}

		HmacState(const void* key_data, size_t key_size) noexcept
		{
			uint8_t header[T::BLOCK_BYTES];
			memset(header, 0, sizeof(header));

			if (key_size <= T::BLOCK_BYTES)
			{
				memcpy(header, key_data, key_size);
			}
			else
			{
				typename T::State st;
				st.append(key_data, key_size);
				st.finalise();
				st.getDigest(header); static_assert(T::DIGEST_BYTES <= T::BLOCK_BYTES);
			}

			for (size_t i = 0; i != sizeof(header); ++i)
			{
				inner.appendByte(header[i] ^ 0x36);
				outer.appendByte(header[i] ^ 0x5c);
			}
		}

		void append(const void* data, size_t size) noexcept
		{
			inner.append(data, size);
		}

		void finalise() noexcept
		{
			uint8_t buf[T::DIGEST_BYTES];
			inner.finalise();
			inner.getDigest(buf);
			outer.append(buf, sizeof(buf));
			outer.finalise();
		}

		void getDigest(uint8_t out[T::DIGEST_BYTES]) const noexcept
		{
			return outer.getDigest(out);
		}

		[[nodiscard]] std::string getDigest() const SOUP_EXCAL
		{
			return outer.getDigest();
		}
	};
}
