#include "drString.hpp"

#include "drDatetime.hpp"
#include "drIpAddr.hpp"
#include "drJsonObject.hpp"
#include "json.hpp"

namespace soup
{
	UniquePtr<drString> drString::reflect(std::string str)
	{
		if (auto res = Datetime::fromIso8601(str.c_str()))
		{
			return soup::make_unique<drDatetime>(std::move(str), std::move(res.value()));
		}
		if (auto res = json::decode(str))
		{
			if (res->isObj())
			{
				return soup::make_unique<drJsonObject>(std::move(str), std::move(res));
			}
		}
		if (IpAddr ip; ip.fromString(str))
		{
			return soup::make_unique<drIpAddr>(std::move(str), std::move(ip));
		}
		return soup::make_unique<drString>(std::move(str));
	}
}
