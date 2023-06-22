#pragma once

namespace soup
{
	template <size_t size>
	struct ByteTraits {};

	template <>
	struct ByteTraits<1>
	{
		using int_t = int8_t;
		using uint_t = uint8_t;
	};

	template <>
	struct ByteTraits<2>
	{
		using int_t = int16_t;
		using uint_t = uint16_t;
	};

	template <>
	struct ByteTraits<4>
	{
		using int_t = int32_t;
		using uint_t = uint32_t;
	};

	template <>
	struct ByteTraits<8>
	{
		using int_t = int64_t;
		using uint_t = uint64_t;
	};
}
