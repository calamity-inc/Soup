#include "drData.hpp"

#include "drString.hpp"

namespace soup
{
	UniquePtr<drData> drData::adaptor_toString(const drData& data, const Capture&)
	{
		return soup::make_unique<drString>(data.toString());
	}

	UniquePtr<drData> drData::adaptor_getTypeName(const drData& data, const Capture&)
	{
		return soup::make_unique<drString>(data.getTypeName());
	}
}
