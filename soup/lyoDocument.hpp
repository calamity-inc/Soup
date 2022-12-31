#pragma once

#include "base.hpp"
#include "fwd.hpp"
#include "lyoContainer.hpp"

#include <string>

#include "lyoStylesheet.hpp"

namespace soup
{
	struct lyoDocument : public lyoContainer
	{
		std::vector<lyoStylesheet> stylesheets;

		lyoDocument();

		[[nodiscard]] static UniquePtr<lyoDocument> fromMarkup(const std::string& markup);
		[[nodiscard]] static UniquePtr<lyoDocument> fromMarkup(const XmlTag& root);

		void propagateStyle();

		[[nodiscard]] bool isValid() const noexcept
		{
			return flat_width != 0;
		}

		void invalidate() noexcept
		{
			flat_width = 0;
		}

		[[nodiscard]] lyoFlatDocument flatten(int width, int height);

#if SOUP_WINDOWS
		[[nodiscard]] Window createWindow(const std::string& title);
#endif
	};
}
