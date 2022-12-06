#pragma once

#include <cfloat>

namespace soup
{
	struct audSound
	{
		[[nodiscard]] virtual double getAmplitude(double t) const noexcept = 0;

		[[nodiscard]] virtual double getDurationSeconds() const noexcept
		{
			return DBL_MAX;
		}
	};
}
