#pragma once

#include <string>
#include <vector>

#include "audSound.hpp"
#include "time.hpp"

namespace soup
{
	struct MorseTiming
	{
		uint16_t unit = 100;

		[[nodiscard]] constexpr uint16_t getDitLength() const noexcept
		{
			return unit;
		}

		[[nodiscard]] constexpr uint16_t getDahLength() const noexcept
		{
			return unit * 3;
		}

		[[nodiscard]] constexpr uint16_t getSpaceLength() const noexcept
		{
			return unit;
		}

		[[nodiscard]] constexpr uint16_t getLetterSpaceLength() const noexcept
		{
			return unit * 3;
		}

		[[nodiscard]] constexpr uint16_t getWordSpaceLength() const noexcept
		{
			return unit * 7;
		}

		[[nodiscard]] constexpr bool isDah(uint16_t dur) const noexcept
		{
			uint16_t cutoff = (getDitLength() + getDahLength()) >> 1;
			return dur > cutoff;
		}

		[[nodiscard]] constexpr bool isLetterSpace(uint16_t dur) const noexcept
		{
			uint16_t cutoff = (getSpaceLength() + getLetterSpaceLength()) >> 1;
			return dur > cutoff;
		}
		
		[[nodiscard]] constexpr bool isWordSpace(uint16_t dur) const noexcept
		{
			uint16_t cutoff = (getLetterSpaceLength() + getWordSpaceLength()) >> 1;
			return dur > cutoff;
		}
	};

	struct MorseSequence : public audSound
	{
		std::vector<uint16_t> alterndur{};

		[[nodiscard]] std::string toPattern() const;
		[[nodiscard]] std::string toPattern(const MorseTiming& timing) const;

		[[nodiscard]] bool isOnAt(std::time_t t) const noexcept;
		[[nodiscard]] std::time_t getDuration() const noexcept;

		// from audSound:
		[[nodiscard]] double getAmplitude(double t) final;
		[[nodiscard]] double getDurationSeconds() noexcept final;
	};

	struct morse
	{
		[[nodiscard]] static const char* encodeLetter(char16_t c);
		[[nodiscard]] static std::string encodeWord(const std::string& word);
		[[nodiscard]] static std::string encode(const std::string& text);

		[[nodiscard]] static std::string decode(const std::string& pattern);

		[[nodiscard]] static MorseSequence patternToSequence(const std::string& pattern);
		[[nodiscard]] static MorseSequence patternToSequence(const std::string& pattern, const MorseTiming& timing);
	};

	struct MorseKey : public audSound
	{
		std::time_t t = 0;
		MorseSequence seq;

		[[nodiscard]] bool isDown() const noexcept
		{
			return seq.alterndur.empty()
				? (t != 0)
				: !(seq.alterndur.size() & 1)
				;
		}

		void down() // on
		{
			if (t != 0)
			{
				seq.alterndur.emplace_back((uint16_t)time::millisSince(t));
			}
			t = time::millis();
		}

		void up() // off
		{
			seq.alterndur.emplace_back((uint16_t)time::millisSince(t));
			t = time::millis();
		}


		// from audSound:
		[[nodiscard]] double getAmplitude(double t) final;
	};
}
