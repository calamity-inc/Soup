#pragma once

#include "audSound.hpp"

#include "aud_common.hpp"
#include "audNote.hpp"

namespace soup
{
	class audNoteEnvelope : public audSound
	{
	protected:
		double angvel;
		double t = 0.0;
		double time_released = 0.0;
	public:
		bool released = false; // this should be set to true some time after playSound

		static constexpr double attack_time = 0.1;
		static constexpr double decay_time = 0.1;
		static constexpr double sustain_amplitude = 0.5;
		static constexpr double release_time = 0.1;

		constexpr audNoteEnvelope(audNote_t note)
			: angvel(HZ_TO_ANGVEL(audNoteToHz(note)))
		{
		}

		void prepare() final;
		[[nodiscard]] bool hasFinished() noexcept final;
		[[nodiscard]] double getAmplitude() final;
	};
}
