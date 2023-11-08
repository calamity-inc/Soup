#pragma once

namespace soup
{
	struct audSound
	{
		virtual ~audSound() = default;

		[[nodiscard]] virtual bool hasFinished() noexcept;

		// Called AUD_SAMPLE_RATE times per second.
		[[nodiscard]] virtual double getAmplitude() = 0;
	};

	struct audSoundSimple : public audSound
	{
		double t = 0.0;

		[[nodiscard]] bool hasFinished() noexcept final;
		[[nodiscard]] double getAmplitude() final;

		[[nodiscard]] virtual double getAmplitude(double t) = 0;
		[[nodiscard]] virtual double getDurationSeconds() noexcept;
	};
}
