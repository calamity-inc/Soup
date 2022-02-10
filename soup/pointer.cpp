#include "pointer.hpp"

#include "module.hpp"

namespace soup
{
	pointer pointer::rip() const noexcept
	{
		return ripT<int32_t>();
	}

#if SOUP_WINDOWS
	pointer pointer::externalRip(const module& mod) const noexcept
	{
		return add(mod.externalRead<int32_t>(*this)).add(sizeof(int32_t));
	}

	pointer pointer::rva(const module& mod) const noexcept
	{
		return pointer(as<std::int32_t&>()).add(mod.base().as<uintptr_t>());
	}

	std::vector<pointer> pointer::getJumps() const noexcept
	{
		auto ptr = *this;
		std::vector<pointer> res{ ptr };
		__try
		{
			while (ptr.as<uint8_t&>() == 0xE9)
			{
				ptr = ptr.add(1).rip();
				res.emplace_back(ptr);
			}
		}
		__except (GetExceptionInformation()->ExceptionRecord->ExceptionCode == STATUS_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
		{
		}
		return res;
	}

	pointer pointer::followJumps() const noexcept
	{
		auto jumps = getJumps();
		return jumps.at(jumps.size() - 1);
	}
#endif
}
