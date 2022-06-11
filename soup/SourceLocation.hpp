#pragma once

namespace soup
{
	struct SourceLocation
	{
		const char* file_name;
		unsigned int line;
	};
}

#define SOUP_CAPTURE_SOURCE_LOCATION ::soup::SourceLocation{ __FILE__, __LINE__ }
