#pragma once

#include "base.hpp"
#if SOUP_WINDOWS

#include <string>

#include <windows.h>

#include "Exception.hpp"
#include "unicode.hpp"

#pragma comment(lib, "advapi32.lib")

NAMESPACE_SOUP
{
	struct osRegistry
	{
		struct Key
		{
			HKEY h;

			~Key()
			{
				if (!isSystem())
				{
					RegCloseKey(h);
				}
			}

			[[nodiscard]] bool isSystem() const noexcept
			{
				return (((LONG)(ULONG_PTR)h) & ((LONG)0x80000000)) != 0;
			}

			[[nodiscard]] bool hasSubkey(const char* name) const noexcept
			{
				HKEY k;
				if (RegOpenKeyA(h, name, &k) == ERROR_SUCCESS)
				{
					RegCloseKey(k);
					return true;
				}
				return false;
			}

			[[nodiscard]] Key getSubkey(const char* name) const
			{
				HKEY k;
				if (RegOpenKeyA(h, name, &k) == ERROR_SUCCESS)
				{
					return Key{ k };
				}
				SOUP_THROW(Exception("Failed to open registry key"));
			}

			[[nodiscard]] Key createSubkey(const char* name) const
			{
				HKEY k;
				if (RegCreateKeyExA(h, name, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &k, nullptr) == ERROR_SUCCESS)
				{
					return Key{ k };
				}
				SOUP_THROW(Exception("Failed to create registry key"));
			}

			void setValue(const std::string& value) const
			{
				setValue(nullptr, value);
			}

			void setValue(const char* key, const std::string& value) const
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
		};

		inline static const Key CLASSES_ROOT{ HKEY_CLASSES_ROOT };;
		inline static const Key CURRENT_USER{ HKEY_CURRENT_USER };;
		inline static const Key LOCAL_MACHINE{ HKEY_LOCAL_MACHINE };;
	};
}

#endif
