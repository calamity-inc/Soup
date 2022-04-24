#include "Oid.hpp"

#include <istream>
#include <sstream>

#include "IstreamReader.hpp"

namespace soup
{
	Oid Oid::fromBinary(const std::string& str)
	{
		std::istringstream s{ str };
		return fromBinary(s);
	}

	Oid Oid::fromBinary(std::istream& s)
	{
		Oid ret{};
		if (auto first = s.get(); first != EOF)
		{
			ret.path.reserve(2);
			ret.path.push_back(first / 40);
			ret.path.push_back(first % 40);
			IstreamReader r(&s);
			while (r.hasMore())
			{
				uint32_t comp;
				r.om<uint32_t>(comp);
				ret.path.emplace_back(comp);
			}
		}
		return ret;
	}

	bool Oid::operator==(const Oid& b) const noexcept
	{
		if (path.size() != b.path.size())
		{
			return false;
		}
		for (auto i = path.rbegin(), j = b.path.rbegin(); i != path.rend(); ++i, ++j)
		{
			if (*i != *j)
			{
				return false;
			}
		}
		return true;
	}

	bool Oid::operator!=(const Oid& b) const noexcept
	{
		return !operator==(b);
	}

	std::string Oid::toString() const
	{
		std::string str{};
		if (auto i = path.begin(); i != path.end())
		{
			while (true)
			{
				str.append(std::to_string(*i));
				if (++i == path.end())
				{
					break;
				}
				str.push_back('.');
			}
		}
		return str;
	}

	std::ostream& operator<<(std::ostream& os, const Oid& v)
	{
		return os << v.toString();
	}
}
