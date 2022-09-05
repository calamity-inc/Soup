#pragma once

#include <memory>
#include <string>

#include "Mixed.hpp"

namespace soup
{
	// Soup Inquiry Language. I would've called it Soup Query Language, but the acronym SQL is already "taken."
	struct InquiryLang
	{
		[[nodiscard]] static std::shared_ptr<Mixed> execute(const std::string& q);
		[[nodiscard]] static std::string formatResult(std::shared_ptr<Mixed> res);
	};
}
