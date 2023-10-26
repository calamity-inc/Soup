#pragma once

#include "AnalogueKeyboard.hpp"
#include "kbRgb.hpp"

namespace soup
{
	// A structure for keyboards that are (at least partially) supported by Soup.
	class Keyboard
	{
	public:
		std::optional<AnalogueKeyboard> analogue;
		UniquePtr<kbRgb> rgb;

		[[nodiscard]] static std::vector<Keyboard> getAll();

		[[nodiscard]] std::string getName() const;

	protected:
		Keyboard(AnalogueKeyboard&& analogue)
			: analogue(std::move(analogue))
		{
		}

		Keyboard(AnalogueKeyboard&& analogue, UniquePtr<kbRgb>&& rgb)
			: analogue(std::move(analogue)), rgb(std::move(rgb))
		{
		}

		Keyboard(UniquePtr<kbRgb>&& rgb)
			: rgb(std::move(rgb))
		{
		}
	};
}
