#pragma once

#include "base.hpp"

NAMESPACE_SOUP
{
	template <typename T>
	struct HmacState
	{
		typename T::State inner;
		typename T::State outer;

		HmacState(std::string key) SOUP_EXCAL
		{
			if (key.length() > T::BLOCK_BYTES)
			{
				key = T::hash(key);
			}

			for (size_t i = 0; i != key.length(); ++i)
			{
				inner.appendByte(key[i] ^ 0x36);
				outer.appendByte(key[i] ^ 0x5c);
			}

			{
				auto diff = T::BLOCK_BYTES - key.length();
				while (diff--)
				{
					inner.appendByte(0x36);
					outer.appendByte(0x5c);
				}
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
