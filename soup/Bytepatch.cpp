#include "Bytepatch.hpp"

#include <cstdlib>
#include <cstring> // memcpy
#include <stdexcept>
#include <string>

#include "macros.hpp" // COUNT
#include "memGuard.hpp"

#if SOUP_WINDOWS
#include <windows.h>
#endif

NAMESPACE_SOUP
{
	Bytepatch::~Bytepatch() noexcept
	{
		restore();
	}

	Bytepatch::Bytepatch(Bytepatch&& b) noexcept
	{
		patchFrom(std::move(b));
	}

	void Bytepatch::operator=(Bytepatch&& b) noexcept
	{
		restore();
		patchFrom(std::move(b));
	}

	void Bytepatch::patchFrom(Bytepatch&& b) noexcept
	{
		if (b.isPatched())
		{
			og_area = b.og_area;
			size = b.size;
			area = b.area;

			b.forget();
		}
	}

	bool Bytepatch::isPatched() const noexcept
	{
		return og_area != nullptr;
	}

	void Bytepatch::forget() noexcept
	{
		og_area = nullptr;
	}

	void Bytepatch::store() noexcept
	{
		og_area = (uint8_t*)malloc(size);
		memcpy(og_area, area, size);
	}

	void Bytepatch::store(uint8_t* area, size_t size) noexcept
	{
		restore();
		this->area = area;
		this->size = size;
		store();
	}

	void Bytepatch::initPatch(uint8_t* area, const uint8_t* patch, size_t size) noexcept
	{
		store(area, size);
#if SOUP_WINDOWS
		DWORD OldProtect;
		VirtualProtect(area, size, PAGE_EXECUTE_READWRITE, &OldProtect);
#else
		memGuard::setAllowedAccess(area, size, memGuard::ACC_RWX);
#endif
		memcpy(area, patch, size);
#if SOUP_WINDOWS
		VirtualProtect(area, size, OldProtect, &OldProtect);
#endif
	}

	bool Bytepatch::initPatchNOP(uint8_t* area, size_t size) noexcept
	{
		if (size == 1)
		{
			store(area, size);
			*area = NOP;
		}
		else if (size == 2)
		{
			uint8_t patch[] = {
				NOP, NOP
			};
			initPatch(area, patch, COUNT(patch));
		}
		else if (size >= 2 && size <= (127 + 2))
		{
			uint8_t patch[] = {
				JMP1, uint8_t(size - 2)
			};
			initPatch(area, patch, COUNT(patch));
		}
		else if (size != 0)
		{
			return false;
		}
		return true;
	}

	void Bytepatch::initPatchZero(uint8_t* area, size_t size) noexcept
	{
		restore();
		this->area = area;
		this->size = size;
		initPatchZero();
	}

	void Bytepatch::initPatchZero() noexcept
	{
		restore();
		store();
		memset(area, 0, size);
	}

	void Bytepatch::restore() noexcept
	{
		if (isPatched())
		{
#if SOUP_WINDOWS
			DWORD OldProtect;
			VirtualProtect(area, size, PAGE_EXECUTE_READWRITE, &OldProtect);
#endif
			memcpy(area, og_area, size);
#if SOUP_WINDOWS
			VirtualProtect(area, size, OldProtect, &OldProtect);
#endif
			free(og_area);
			forget();
		}
	}

	void Bytepatch::dispose() noexcept
	{
		if (isPatched())
		{
			free(og_area);
			forget();
		}
	}
}
