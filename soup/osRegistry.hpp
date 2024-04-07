#pragma once

#include <string>

#include <Windows.h>

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

			[[nodiscard]] bool hasSubkey(const char* name) const noexcept;
			[[nodiscard]] Key getSubkey(const char* name) const;
			[[nodiscard]] Key createSubkey(const char* name) const;

			void setValue(const std::string& value) const;
			void setValue(const char* key, const std::string& value) const;
		};

		static Key CLASSES_ROOT;
		static Key CURRENT_USER;
		static Key LOCAL_MACHINE;
	};
}
