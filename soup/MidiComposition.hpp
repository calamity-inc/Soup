#pragma once

#include <vector>

#include "audNote.hpp"
#include "fwd.hpp"

namespace soup
{
	enum MidiEventType : uint8_t
	{
		MIDI_NOTE_OFF,
		MIDI_NOTE_ON,
	};

	struct MidiComposition
	{
		struct Event
		{
			double t;
			MidiEventType type;
			audNote_t note;

			Event(double t, MidiEventType type, audNote_t note)
				: t(t), type(type), note(note)
			{
			}
		};

		std::vector<Event> events{};

		MidiComposition(Reader& r);
	};
}
