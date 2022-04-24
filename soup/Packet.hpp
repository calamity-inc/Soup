#pragma once

#define SOUP_PACKET(name) struct name : public ::soup::Packet<name>
#define SOUP_PACKET_IO(s) template <typename T> bool io(T& s)

#include "IstreamReader.hpp"
#include "OstreamWriter.hpp"
#include "StringReader.hpp"
#include "StringWriter.hpp"

#include <sstream>

namespace soup
{
	template <typename T>
	class Packet
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
			StringReader r(std::move(bin), false);
			return read(r);
		}

		bool fromBinaryLE(std::string bin)
		{
			StringReader r(std::move(bin), true);
			return read(r);
		}

		bool read(std::istream& is)
		{
			IstreamReader r(&is, false);
			return read(r);
		}

		bool readLE(std::istream& is)
		{
			IstreamReader r(&is, true);
			return read(r);
		}

		bool read(Reader& r)
		{
			return reinterpret_cast<T*>(this)->template io<Reader>(r);
		}

		[[nodiscard]] std::string toBinary()
		{
			StringWriter w(false);
			write(w);
			return w.str;
		}

		[[nodiscard]] std::string toBinaryLE()
		{
			StringWriter w(true);
			write(w);
			return w.str;
		}

		bool write(std::ostream& os)
		{
			OstreamWriter w(&os, false);
			return write(w);
		}

		bool writeLE(std::ostream& os)
		{
			OstreamWriter w(&os, true);
			return write(w);
		}

		bool write(Writer& w)
		{
			return reinterpret_cast<T*>(this)->template io<Writer>(w);
		}
	};
}
