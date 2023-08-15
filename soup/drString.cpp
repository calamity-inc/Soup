#include "drString.hpp"

#include "drDatetime.hpp"
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
		return soup::make_unique<drString>(std::move(str));
	}
}
