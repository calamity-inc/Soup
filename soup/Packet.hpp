#pragma once

#define SOUP_PACKET(name) struct name : public ::soup::Packet<name>
#define SOUP_PACKET_IO(s) template <typename T> bool io(T& s)
#define SOUP_IF_ISREAD if constexpr (T::isRead())
#define SOUP_ELSEIF_ISWRITE else

#include "BufferWriter.hpp"
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
		bool fromBinary(std::string bin, Endian endian = BIG_ENDIAN)
		{
			StringReader r(std::move(bin), endian);
			return read(r);
		}

		bool fromBinaryLE(std::string bin)
		{
			StringReader r(std::move(bin), LITTLE_ENDIAN);
			return read(r);
		}

		bool read(std::istream& is, Endian endian = BIG_ENDIAN)
		{
			IstreamReader r(is, endian);
			return read(r);
		}

		bool readLE(std::istream& is)
		{
			return read(is, LITTLE_ENDIAN);
		}

		template <typename Reader = Reader>
		bool read(Reader& r)
		{
			return reinterpret_cast<T*>(this)->template io<Reader>(r);
		}

		[[nodiscard]] Buffer toBinary(Endian endian = BIG_ENDIAN)
		{
			BufferWriter w(endian);
			write(w);
			return w.buf;
		}

		[[nodiscard]] std::string toBinaryString(Endian endian = BIG_ENDIAN)
		{
			StringWriter w(endian);
			write(w);
			return w.data;
		}

		[[nodiscard]] std::string toBinaryStringLE()
		{
			return toBinaryString(LITTLE_ENDIAN);
		}

		bool write(std::ostream& os, Endian endian = BIG_ENDIAN)
		{
			OstreamWriter w(os, endian);
			return write(w);
		}

		bool writeLE(std::ostream& os)
		{
			return write(os, LITTLE_ENDIAN);
		}

		template <typename Writer = Writer>
		bool write(Writer& w)
		{
			return reinterpret_cast<T*>(this)->template io<Writer>(w);
		}
	};
}
