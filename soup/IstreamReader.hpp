#pragma once

#include "Reader.hpp"

#include <istream>

namespace soup
{
	class IstreamReader final : public Reader
	{
	public:
		std::istream* is;

		IstreamReader(std::istream* is, Endian endian = LITTLE_ENDIAN)
			: Reader(endian), is(is)
		{
		}

		IstreamReader(std::istream* is, bool little_endian)
			: Reader(little_endian), is(is)
		{
		}

		~IstreamReader() final = default;

		bool hasMore() final
		{
			return is->peek() != EOF;
		}

		bool u8(uint8_t& p) final
		{
			return !is->read((char*)&p, sizeof(uint8_t)).bad();
		}

	protected:
		bool str_impl(std::string& v, size_t len) final
		{
			v = std::string(len, 0);
			is->read(v.data(), len);
			return !is->bad();
		}
	};
}
