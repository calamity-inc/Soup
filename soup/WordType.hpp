#pragma once

#include <cstdint>

#include "base.hpp"

NAMESPACE_SOUP
{
	// aka. Types of Part of Speech
	enum WordType : uint8_t
	{
		NOUN = 0,
		ADJECTIVE,
		VERB,
		ADVERB,
	};
}
