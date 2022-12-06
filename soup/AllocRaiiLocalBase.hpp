#pragma once

namespace soup
{
	struct AllocRaiiLocalBase
	{
		void* addr;

		AllocRaiiLocalBase() noexcept
			: addr(nullptr)
		{
		}

		AllocRaiiLocalBase(void* data) noexcept
			: addr(data)
		{
		}

		AllocRaiiLocalBase(const AllocRaiiLocalBase&) = delete;
		AllocRaiiLocalBase(AllocRaiiLocalBase&&) = delete;
	};
}
