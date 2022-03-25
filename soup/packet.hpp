#pragma once

#define SOUP_PACKET(name) struct name : public ::soup::packet<name>
#define SOUP_PACKET_IO(s) template <typename T> bool io(T& s)

#include "packet_reader.hpp"
#include "packet_writer.hpp"

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
		bool fromBinary(const std::string& bin)
		{
			std::istringstream iss{ bin };
			return read(iss);
		}

		bool fromBinaryLE(const std::string& bin)
		{
			std::istringstream iss{ bin };
			return readLE(iss);
		}

		[[nodiscard]] std::string toBinary()
		{
			std::ostringstream oss{};
			write(oss);
			return oss.str();
		}

		[[nodiscard]] std::string toBinaryLE()
		{
			std::ostringstream oss{};
			writeLE(oss);
			return oss.str();
		}

		bool read(std::basic_istream<char, std::char_traits<char>>& is)
		{
			packet_reader<false> r(&is);
			return reinterpret_cast<T*>(this)->template io<packet_reader<false>>(r);
		}

		bool readLE(std::basic_istream<char, std::char_traits<char>>& is)
		{
			packet_reader<true> r(&is);
			return reinterpret_cast<T*>(this)->template io<packet_reader<true>>(r);
		}

		bool write(std::basic_ostream<char, std::char_traits<char>>& os)
		{
			packet_writer<false> w(&os);
			return reinterpret_cast<T*>(this)->template io<packet_writer<false>>(w);
		}

		bool writeLE(std::basic_ostream<char, std::char_traits<char>>& os)
		{
			packet_writer<true> w(&os);
			return reinterpret_cast<T*>(this)->template io<packet_writer<true>>(w);
		}
	};
}
