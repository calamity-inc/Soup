#pragma once

#define SOUP_PACKET(name) struct name : public ::soup::packet<name>
#define SOUP_PACKET_IO(s) template <typename T> bool io(T& s)

#include "istream_reader.hpp"
#include "ostream_writer.hpp"
#include "string_reader.hpp"
#include "string_writer.hpp"

#include <sstream>

namespace soup
{
	template <typename T>
	class packet
	{
	protected:
#include "shortint_impl.hpp"
		using u24 = u32;
		using u40 = u64;
		using u48 = u64;
		using u56 = u64;

	public:
		bool fromBinary(std::string bin)
		{
			string_reader r(std::move(bin), false);
			return read(r);
		}

		bool fromBinaryLE(std::string bin)
		{
			string_reader r(std::move(bin), true);
			return read(r);
		}

		bool read(std::istream& is)
		{
			istream_reader r(&is, false);
			return read(r);
		}

		bool readLE(std::istream& is)
		{
			istream_reader r(&is, true);
			return read(r);
		}

		bool read(reader& r)
		{
			return reinterpret_cast<T*>(this)->template io<reader>(r);
		}

		[[nodiscard]] std::string toBinary()
		{
			string_writer w(false);
			write(w);
			return w.str;
		}

		[[nodiscard]] std::string toBinaryLE()
		{
			string_writer w(true);
			write(w);
			return w.str;
		}

		bool write(std::ostream& os)
		{
			ostream_writer w(&os, false);
			return write(w);
		}

		bool writeLE(std::ostream& os)
		{
			ostream_writer w(&os, true);
			return write(w);
		}

		bool write(writer& w)
		{
			return reinterpret_cast<T*>(this)->template io<writer>(w);
		}
	};
}
