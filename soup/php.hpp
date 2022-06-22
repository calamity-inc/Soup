#pragma once

#include <string>

namespace soup
{
	class php
	{
	public:
		[[nodiscard]] static std::string evaluate(const std::string& code);
	private:
		[[nodiscard]] static std::string evaluatePhpmode(const std::string& code, std::string::const_iterator& i);
	public:
		[[nodiscard]] static std::string evaluatePhp(const std::string& code);
	};
}
