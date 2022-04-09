#include "oid.hpp"

#include <istream>
#include <sstream>

#include "istream_reader.hpp"

namespace soup
{
	oid oid::fromBinary(const std::string& str)
	{
		std::istringstream s{ str };
		return fromBinary(s);
	}

	oid oid::fromBinary(std::istream& s)
	{
		oid ret{};
		if (auto first = s.get(); first != EOF)
		{
			ret.path.reserve(2);
			ret.path.push_back(first / 40);
			ret.path.push_back(first % 40);
			istream_reader r(&s);
			while (r.hasMore())
			{
				uint32_t comp;
				r.om<uint32_t>(comp);
				ret.path.emplace_back(comp);
			}
		}
		return ret;
	}

	bool oid::operator==(const oid& b) const noexcept
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

	bool oid::operator!=(const oid& b) const noexcept
	{
		return !operator==(b);
	}

	std::string oid::toString() const
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

	std::ostream& operator<<(std::ostream& os, const oid& v)
	{
		return os << v.toString();
	}
}
