#pragma once

#include "base.hpp"

namespace soup
{
	enum class audNote : uint8_t
	{
		C4,
		CSHARP4,
		D4,
		DSHARP4,
		E4,
		F4,
		FSHARP4,
		G4,
		GSHARP4,
		A4,
		ASHARP4,
		B4,
		C5,
		CSHARP5,
		D5,
		DSHARP5,
		E5,
		F5,
		FSHARP5,
		G5,
		GSHARP5,
		A5,
		ASHARP5,
		B5,

		SIZE
	};

	[[nodiscard]] constexpr float audNoteToHz(audNote note)
	{
		switch (note)
		{
		case audNote::C4: return 261.63f;
		case audNote::CSHARP4: return 277.18f;
		case audNote::D4: return 293.66f;
		case audNote::DSHARP4: return 311.13f;
		case audNote::E4: return 329.63f;
		case audNote::F4: return 349.23f;
		case audNote::FSHARP4: return 369.99f;
		case audNote::G4: return 392.00f;
		case audNote::GSHARP4: return 415.30f;
		case audNote::A4: return 440.00f;
		case audNote::ASHARP4: return 466.16f;
		case audNote::B4: return 493.88f;
		case audNote::C5: return 523.25f;
		case audNote::CSHARP5: return 554.37f;
		case audNote::D5: return 587.33f;
		case audNote::DSHARP5: return 622.25f;
		case audNote::E5: return 659.26f;
		case audNote::F5: return 698.46f;
		case audNote::FSHARP5: return 739.99f;
		case audNote::G5: return 783.99f;
		case audNote::GSHARP5: return 830.61f;
		case audNote::A5: return 880.00f;
		case audNote::ASHARP5: return 932.33f;
		case audNote::B5: return 987.77f;
		case audNote::SIZE:;
		}
		SOUP_ASSERT_UNREACHABLE;
	}
}
