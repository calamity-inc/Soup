#include "osInput.hpp"
#if SOUP_WINDOWS

#include <windows.h>

#include "Key.hpp"

NAMESPACE_SOUP
{
	void osInput::simulateKeyPress(Key key)
	{
		return simulateKeyPress(std::vector<Key>{ key });
	}

	void osInput::simulateKeyPress(bool ctrl, bool shift, bool alt, Key key)
	{
		return simulateKeyPress(ctrl, shift, alt, false, key);
	}

	void osInput::simulateKeyPress(bool ctrl, bool shift, bool alt, bool meta, Key key)
	{
		std::vector<Key> keys{};
		keys.reserve(5);
		if (ctrl) keys.emplace_back(KEY_LCTRL);
		if (shift) keys.emplace_back(KEY_LSHIFT);
		if (alt) keys.emplace_back(KEY_LALT);
		if (meta) keys.emplace_back(KEY_LMETA);
		keys.emplace_back(key);
		simulateKeyPress(keys);
	}

	void osInput::simulateKeyPress(const std::vector<Key>& keys)
	{
		for (auto i = keys.cbegin(); i != keys.cend(); ++i)
		{
			simulateKeyDown(*i);
		}

		for (auto i = keys.crbegin(); i != keys.crend(); ++i)
		{
			simulateKeyRelease(*i);
		}
	}

	void osInput::simulateKeyDown(Key key)
	{
		INPUT input;
		input.type = INPUT_KEYBOARD;
		input.ki.wVk = soup_key_to_virtual_key(key);
		input.ki.wScan = soup_key_to_ps2_scancode(key);
		input.ki.dwFlags = 0;
		if (input.ki.wScan & 0xE000)
		{
			input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
		}
		input.ki.time = 0;
		input.ki.dwExtraInfo = 0;
		SendInput(1, &input, sizeof(INPUT));
	}

	void osInput::simulateKeyRelease(Key key)
	{
		INPUT input;
		input.type = INPUT_KEYBOARD;
		input.ki.wVk = soup_key_to_virtual_key(key);
		input.ki.wScan = soup_key_to_ps2_scancode(key);
		input.ki.dwFlags = KEYEVENTF_KEYUP;
		if (input.ki.wScan & 0xE000)
		{
			input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
		}
		input.ki.time = 0;
		input.ki.dwExtraInfo = 0;
		SendInput(1, &input, sizeof(INPUT));
	}
}

#endif
