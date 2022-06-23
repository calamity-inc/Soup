#pragma once

#include <filesystem>
#include <string>

namespace soup
{
	class PhpState
	{
	public:
		std::filesystem::path cwd;

		[[nodiscard]] std::string evaluate(const std::string& code, unsigned int max_require_depth = 10) const;
	private:
		[[nodiscard]] std::string evaluatePhpmode(const std::string& code, std::string::const_iterator& i, unsigned int max_require_depth) const;
	public:
		[[nodiscard]] std::string evaluatePhp(const std::string& code, unsigned int max_require_depth = 10) const;
	};
}
