#pragma once

#include <string>

#include <Windows.h>

namespace soup
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
			[[nodiscard]] Key createSubkey(const char* name);

			void setValue(const std::string& value);
			void setValue(const char* key, const std::string& value) const;
		};

		static Key CLASSES_ROOT;
		static Key CURRENT_USER;
		static Key LOCAL_MACHINE;
	};
}
