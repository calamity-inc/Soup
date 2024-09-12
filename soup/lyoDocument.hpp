#pragma once

#include "base.hpp"
#include "fwd.hpp"
#include "lyoContainer.hpp"

#include <string>

#include "lyoStylesheet.hpp"

NAMESPACE_SOUP
{
	struct lyoDocument : public lyoContainer
	{
		std::vector<lyoStylesheet> stylesheets;
		lyoElement* focus = nullptr;

		lyoDocument();

		[[nodiscard]] static UniquePtr<lyoDocument> fromMarkup(const std::string& markup);
		[[nodiscard]] static UniquePtr<lyoDocument> fromMarkup(const XmlTag& root);

		void propagateStyle() final;

		[[nodiscard]] bool isValid() const noexcept
		{
			return flat_width != 0;
		}

		void invalidate() noexcept
		{
			flat_width = 0;
		}

		[[nodiscard]] lyoFlatDocument flatten(unsigned int width, unsigned int height);

		[[nodiscard]] Window createWindow(const std::string& title);
	};
}
