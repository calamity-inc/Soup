#pragma once

namespace soup
{
	struct audSound
	{
		[[nodiscard]] virtual double getAmplitude(double t) const noexcept = 0;
		[[nodiscard]] virtual double getDuration() const noexcept = 0;
	};
}
