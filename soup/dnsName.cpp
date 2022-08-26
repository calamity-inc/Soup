#include "dnsName.hpp"

#include "StringPtrReader.hpp"

namespace soup
{
	std::vector<std::string> dnsName::resolve(const std::string& data) const
	{
		std::vector<std::string> res = name;
		if (ptr != 0)
		{
			StringPtrReader sr(&data, false);
			sr.offset = ptr;

			dnsName cont;
			cont.read(sr);

			auto vec = cont.resolve(data);
			res.insert(res.end(), std::make_move_iterator(vec.begin()), std::make_move_iterator(vec.end()));
		}
		return res;
	}
}
