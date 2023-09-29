#pragma once

#include "HandleBase.hpp"

namespace soup
{
	struct HandleRaii final : public HandleBase
	{
		using HandleBase::HandleBase;

		HandleRaii(HANDLE h) noexcept
			: HandleBase(h)
		{
		}

		HandleRaii(HandleRaii&& b) noexcept
			: HandleBase(b.h)
		{
			b.h = INVALID_HANDLE_VALUE;
		}

		~HandleRaii() noexcept final
		{
			if (isValid())
			{
				CloseHandle(h);
				invalidate();
			}
		}

		using HandleBase::operator=;
	};
}
