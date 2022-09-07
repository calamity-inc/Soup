#pragma once

#include <string>

#include "Mixed.hpp"
#include "SharedPtr.hpp"

namespace soup
{
	// Soup Inquiry Language. I would've called it Soup Query Language, but the acronym SQL is already "taken."
	struct InquiryLang
	{
		[[nodiscard]] static SharedPtr<Mixed> execute(const std::string& q);
		[[nodiscard]] static std::string formatResult(SharedPtr<Mixed> res);
		[[nodiscard]] static std::string formatResult(const Mixed& res);
		[[nodiscard]] static std::string formatResultLine(const Mixed& res);
	};
}
