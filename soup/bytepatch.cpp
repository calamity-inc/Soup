#include "bytepatch.hpp"

#include <cstdlib>
#include <cstring> // memcpy
#include <stdexcept>
#include <string>

#include "macros.hpp"

namespace soup
{
	bytepatch::~bytepatch()
	{
		restore();
	}

	bytepatch::bytepatch(bytepatch&& b)
	{
		patchFrom(std::move(b));
	}

	void bytepatch::operator=(bytepatch&& b)
	{
		restore();
		patchFrom(std::move(b));
	}

	void bytepatch::patchFrom(bytepatch&& b)
	{
		if (b.isPatched())
		{
			og_area = b.og_area;
			size = b.size;
			area = b.area;

			b.dispose();
		}
	}

	bool bytepatch::isPatched() const
	{
		return og_area != nullptr;
	}

	void bytepatch::dispose()
	{
		og_area = nullptr;
	}

	void bytepatch::store()
	{
		og_area = (uint8_t*)malloc(size);
		memcpy(og_area, area, size);
	}

	void bytepatch::store(uint8_t* area, size_t size)
	{
		restore();
		this->area = area;
		this->size = size;
		store();
	}

	void bytepatch::initPatch(uint8_t* area, const uint8_t* patch, size_t size)
	{
		store(area, size);
		memcpy(area, patch, size);
	}

	bool bytepatch::initPatchNOP(uint8_t* area, size_t size)
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

	void bytepatch::initPatchZero(uint8_t* area, size_t size)
	{
		restore();
		this->area = area;
		this->size = size;
		initPatchZero();
	}

	void bytepatch::initPatchZero()
	{
		restore();
		store();
		memset(area, 0, size);
	}

	void bytepatch::restore()
	{
		if (isPatched())
		{
			memcpy(area, og_area, size);
			free(og_area);
			dispose();
		}
	}
}
