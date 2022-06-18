#pragma once

namespace soup
{
	struct SourceLocation
	{
		const char* file_name = nullptr;
		unsigned int line = 0;

		[[nodiscard]] bool isValid() const noexcept
		{
			return file_name != nullptr;
		}
	};
}

#define SOUP_CAPTURE_SOURCE_LOCATION ::soup::SourceLocation{ __FILE__, __LINE__ }
