#include "Oid.hpp"

#include <istream>
#include <sstream>

#include "IstreamReader.hpp"
#include "StringRefWriter.hpp"

namespace soup
{
	Oid Oid::COMMON_NAME = { 2, 5, 4, 3 };
	Oid Oid::RSA_ENCRYPTION = { 1, 2, 840, 113549, 1, 1, 1 };
	Oid Oid::SHA256_WITH_RSA_ENCRYPTION = { 1, 2, 840, 113549, 1, 1, 11 };
	Oid Oid::SUBJECT_ALT_NAME = { 2, 5, 29, 17 };

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
			IstreamReader r(s);
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

	std::string Oid::toDer() const
	{
		std::string res;
		res.push_back((char)((path.at(0) * 40) | path.at(1)));
		StringRefWriter w(res);
		for (auto i = path.begin() + 2; i != path.end(); ++i)
		{
			w.om<uint32_t>(*i);
		}
		return res;
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
