#include "string.hpp"

#include <filesystem>
#include <fstream>
#include <streambuf>

namespace soup
{
	void string::listAppend(std::string& str, std::string&& add)
	{
		if (str.empty())
		{
			str = std::move(add);
		}
		else
		{
			str.append(", ").append(add);
		}
	}

	std::string string::fromFile(const std::string& file)
	{
		std::string ret{};
		if (std::filesystem::exists(file))
		{
			std::ifstream t(file);

			t.seekg(0, std::ios::end);
			ret.reserve(t.tellg());
			t.seekg(0, std::ios::beg);

			ret.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
		}
		return ret;
	}
}
