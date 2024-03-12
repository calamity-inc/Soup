#pragma once

#include "ioSizeMeasurer.hpp"
#include "Reader.hpp"

namespace soup
{
	template <typename T>
	[[nodiscard]] size_t measurePacket()
	{
		T inst;
		ioSizeMeasurer sm;
		inst.write(sm);
		return sm.size;
	}

	// Requires C++20 or higher.
	// All entries are assumed to have the same size.
	struct DiskDbReader
	{
		Reader& r;
		size_t entry_size;

		DiskDbReader(Reader& r, size_t entry_size)
			: r(r), entry_size(entry_size)
		{
		}

		[[nodiscard]] size_t getNumEntries() const
		{
			r.seekEnd();
			return r.getPosition() / entry_size;
		}

		// cmp function should read (partial) entry and return hint <=> ...
		template <typename T>
		bool seekEntry(const T& hint, std::strong_ordering(*cmp)(Reader& r, const T& hint))
		{
			size_t start = 0;
			size_t end = getNumEntries();
			for (size_t distance; distance = (end - start), distance > 1; )
			{
				size_t pivot = (start + (distance / 2));
				r.seek(pivot * entry_size);
				auto res = cmp(r, hint);
				if (res == 0)
				{
					r.seek(pivot * entry_size);
					return true;
				}
				if (res < 0)
				{
					end = pivot;
				}
				else
				{
					start = pivot;
				}
			}
			return false;
		}
	};
}
