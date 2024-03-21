#include "base.hpp"
#if SOUP_WINDOWS

#include "osRegistry.hpp"

#include "Exception.hpp"
#include "unicode.hpp"

#pragma comment(lib, "Advapi32")

namespace soup
{
	bool osRegistry::Key::hasSubkey(const char* name) const noexcept
	{
		HKEY k;
		if (RegOpenKeyA(h, name, &k) == ERROR_SUCCESS)
		{
			RegCloseKey(k);
			return true;
		}
		return false;
	}

	osRegistry::Key osRegistry::Key::getSubkey(const char* name) const
	{
		HKEY k;
		if (RegOpenKeyA(h, name, &k) == ERROR_SUCCESS)
		{
			return Key{ k };
		}
		SOUP_THROW(Exception("Failed to open registry key"));
	}

	osRegistry::Key osRegistry::Key::createSubkey(const char* name) const
	{
		HKEY k;
		if (RegCreateKeyExA(h, name, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &k, nullptr) == ERROR_SUCCESS)
		{
			return Key{ k };
		}
		SOUP_THROW(Exception("Failed to create registry key"));
	}

	void osRegistry::Key::setValue(const std::string& value) const
	{
		setValue(nullptr, value);
	}

	void osRegistry::Key::setValue(const char* key, const std::string& value) const
	{
		auto value_utf16 = unicode::utf8_to_utf16(value);

		if (key == nullptr)
		{
			RegSetValueExW(h, nullptr, 0, REG_SZ, (const BYTE*)value_utf16.data(), static_cast<DWORD>(value_utf16.size() * sizeof(UTF16_CHAR_TYPE)));
		}
		else
		{
			auto key_utf16 = unicode::utf8_to_utf16(key);
			RegSetValueExW(h, key_utf16.c_str(), 0, REG_SZ, (const BYTE*)value_utf16.data(), static_cast<DWORD>(value_utf16.size() * sizeof(UTF16_CHAR_TYPE)));
		}
	}

	osRegistry::Key osRegistry::CLASSES_ROOT{ HKEY_CLASSES_ROOT };
	osRegistry::Key osRegistry::CURRENT_USER{ HKEY_CURRENT_USER };
	osRegistry::Key osRegistry::LOCAL_MACHINE{ HKEY_LOCAL_MACHINE };
}

#endif
