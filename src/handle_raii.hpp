#pragma once

#include "handle_base.hpp"

namespace soup
{
	struct handle_raii final : public handle_base
	{
		using handle_base::handle_base;

		~handle_raii() noexcept final
		{
			if (valid())
			{
				CloseHandle(h);
				invalidate();
			}
		}
	};
}
