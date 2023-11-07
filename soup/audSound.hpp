#pragma once

#include <cfloat>

namespace soup
{
	struct audSound
	{
		virtual ~audSound() = default;

		[[nodiscard]] virtual double getAmplitude(double t) = 0;

		[[nodiscard]] virtual double getDurationSeconds() noexcept
		{
			return DBL_MAX;
		}
	};
}
