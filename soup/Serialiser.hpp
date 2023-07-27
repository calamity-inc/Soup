#pragma once

#include "fwd.hpp"

namespace soup
{
	template <typename T>
	struct Serialiser
	{
		bool operator() (Reader& s, T& t)
		{
			return t.io(s);
		}

		bool operator() (Writer& s, T& t)
		{
			return t.io(s);
		}
	};

	template <>
	struct Serialiser<uint64_t>
	{
		bool operator() (Reader& s, uint64_t& t)
		{
			return s.u64_dyn(t);
		}

		bool operator() (Writer& s, uint64_t& t)
		{
			return s.u64_dyn(t);
		}
	};
	
	template <>
	struct Serialiser<std::string>
	{
		bool operator() (Reader& s, std::string& t)
		{
			return s.str_lp_u64_dyn(t);
		}

		bool operator() (Writer& s, std::string& t)
		{
			return s.str_lp_u64_dyn(t);
		}
	};

	template <typename T>
	bool ioSerialise(Reader& s, T& t)
	{
		Serialiser<T> ser;
		return ser(s, t);
	}

	template <typename T>
	bool ioSerialise(Writer& s, T& t)
	{
		Serialiser<T> ser;
		return ser(s, t);
	}
}
