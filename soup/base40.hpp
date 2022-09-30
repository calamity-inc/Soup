#pragma once

#include "CustomEncoding.hpp"

namespace soup
{
	// Base40: A human-writable binary data encoding.
	//
	// The alphabet of this encoding is based on the alphanumeric characters:
	//
	// ABCDEFGHIJKLMNOPQRSTUVWXYZ
	// abcdefghijklmnopqrstuvwxyz
	// 0123456789
	//
	// Having written them all down, I identified the following possible ambiguities:
	//
	// B 8
	// C c
	// G 6
	// I l
	// i j
	// O o 0
	// S s 5
	// U u
	// V v
	// X x
	// Z z
	//
	// This might be a bit too harsh, since a lowercase "s" might not be ambigious depending on the style of writing,
	// or we want to assume the human will appropriately size all their letters so some of the lowercase variants would be valid,
	// so I encourage you to use "CustomEncoding" and come up with your own alphabets.
	//
	// Anyway, with my choice of ambigious characters, we get the following 40-character alphabet:
	//
	// ADEFHJKLMNPQRTWYabdefghikmnpqrtuwy123479

#if SOUP_CPP20
	using base40 = CustomEncoding<"ADEFHJKLMNPQRTWYabdefghikmnpqrtuwy123479">;
#else
	struct base40
	{
		[[nodiscard]] static std::string encode(const std::string& msg)
		{
			return encode(Bigint::fromBinary(msg));
		}

		[[nodiscard]] static std::string encode(const Bigint& msg)
		{
			CustomEncoding enc("ADEFHJKLMNPQRTWYabdefghikmnpqrtuwy123479");
			return enc.encode(msg);
		}

		[[nodiscard]] static std::string decode(const std::string& msg)
		{
			CustomEncoding enc("ADEFHJKLMNPQRTWYabdefghikmnpqrtuwy123479");
			return enc.decode(msg);
		}
	};
#endif
}
