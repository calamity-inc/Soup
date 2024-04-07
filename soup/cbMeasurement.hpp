#pragma once

#include <cstdint>
#include <string>

#include "base.hpp"

NAMESPACE_SOUP
{
	enum cbUnit : uint8_t
	{
		CB_NOUNIT,

		CB_CM,
		CB_M,
		CB_KM,
		CB_IN,
		CB_FT,
		CB_YARDS,
		CB_RODS,
		CB_CHAINS,
		CB_FURLONGS,
		CB_MI,

		CB_G,
		CB_KG,
		CB_OUNCES,
		CB_POUNDS,
		CB_STONES,
		CB_HUNDREDWEIGHTS,
		CB_USTONS,
		CB_METRICTONS,
		CB_IMPERIALTONS,
	};

	[[nodiscard]] constexpr bool cbUnitIsDistance(cbUnit unit) noexcept
	{
		return unit >= CB_CM && unit <= CB_MI;
	}

	[[nodiscard]] constexpr bool cbUnitIsWeight(cbUnit unit) noexcept
	{
		return unit >= CB_G && unit <= CB_IMPERIALTONS;
	}

	[[nodiscard]] const char* cbUnitToString(cbUnit unit);
	[[nodiscard]] cbUnit cbUnitFromString(std::string str);

	[[nodiscard]] double cbUnitGetFactor(cbUnit unit) noexcept;

	struct cbMeasurement
	{
		double quantity;
		cbUnit unit;

		[[nodiscard]] double getQuantityIn(cbUnit to_unit) const noexcept
		{
			return quantity * cbUnitGetFactor(unit) / cbUnitGetFactor(to_unit);
		}

		[[nodiscard]] std::string toString() const;
	};
}
