#include "drDatetime.hpp"

#include "drInt.hpp"

NAMESPACE_SOUP
{
	std::vector<drAdaptor> drDatetime::getAdaptors() const
	{
		auto adaptors = drString::getAdaptors();
		adaptors.emplace_back(drAdaptor{"day", [](const drData& data, const Capture&) -> UniquePtr<drData>
		{
			return soup::make_unique<drInt>(static_cast<const drDatetime&>(data).datetime_data.day);
		}});
		adaptors.emplace_back(drAdaptor{"month", [](const drData& data, const Capture&) -> UniquePtr<drData>
		{
			return soup::make_unique<drInt>(static_cast<const drDatetime&>(data).datetime_data.month);
		}});
		adaptors.emplace_back(drAdaptor{"year", [](const drData& data, const Capture&) -> UniquePtr<drData>
		{
			return soup::make_unique<drInt>(static_cast<const drDatetime&>(data).datetime_data.year);
		}});
		adaptors.emplace_back(drAdaptor{"hour", [](const drData& data, const Capture&) -> UniquePtr<drData>
		{
			return soup::make_unique<drInt>(static_cast<const drDatetime&>(data).datetime_data.hour);
		}});
		adaptors.emplace_back(drAdaptor{"minute", [](const drData& data, const Capture&) -> UniquePtr<drData>
		{
			return soup::make_unique<drInt>(static_cast<const drDatetime&>(data).datetime_data.minute);
		}});
		adaptors.emplace_back(drAdaptor{"second", [](const drData& data, const Capture&) -> UniquePtr<drData>
		{
			return soup::make_unique<drInt>(static_cast<const drDatetime&>(data).datetime_data.second);
		}});
		adaptors.emplace_back(drAdaptor{"timestamp", [](const drData& data, const Capture&) -> UniquePtr<drData>
		{
			return soup::make_unique<drInt>(static_cast<const drDatetime&>(data).datetime_data.toTimestamp());
		}});
		return adaptors;
	}
}
