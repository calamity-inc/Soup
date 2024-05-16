#pragma once

#include "base.hpp"
#include "fwd.hpp"

#include <vector>

NAMESPACE_SOUP
{
	struct osInput
	{
#if SOUP_WINDOWS
		static void simulateKeyPress(Key key);
		static void simulateKeyPress(bool ctrl, bool shift, bool alt, Key key);
		static void simulateKeyPress(bool ctrl, bool shift, bool alt, bool meta, Key key);
		static void simulateKeyPress(const std::vector<Key>& keys);
		static void simulateKeyDown(Key key);
		static void simulateKeyRelease(Key key);
#endif
	};
}
