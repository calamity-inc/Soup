#pragma once

#include "reader.hpp"

#include <istream>

namespace soup
{
	class istream_reader final : public reader
	{
	public:
		std::istream* is;

		istream_reader(std::istream* is, bool little_endian = true)
			: reader(little_endian), is(is)
		{
		}

		~istream_reader() final = default;

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
