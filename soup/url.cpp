#include "url.hpp"

#include "base64.hpp"

namespace soup
{
	std::string url::data(const char* mime_type, const std::string& contents)
	{
		std::string url = "data:";
		url.append(mime_type);
		url.append(";base64,");
		url.append(base64::encode(contents));
		return url;
	}
}
