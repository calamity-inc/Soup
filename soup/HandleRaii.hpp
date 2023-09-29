#pragma once

#include "HandleBase.hpp"

namespace soup
{
	struct HandleRaii final : public HandleBase
	{
		using HandleBase::HandleBase;

		~HandleRaii() noexcept final
		{
			if (isValid())
			{
				CloseHandle(h);
				invalidate();
			}
		}
	};
}
