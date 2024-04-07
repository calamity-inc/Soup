#include "drData.hpp"

#include "drString.hpp"

NAMESPACE_SOUP
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
