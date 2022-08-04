#pragma once

#include "base.hpp"
#include "fwd.hpp"
#include "lyoContainer.hpp"

#include <string>

namespace soup
{
	struct lyoDocument : public lyoContainer
	{
		lyoDocument()
			: lyoContainer(nullptr)
		{
			setMargin(8);
		}

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
