#pragma once

#include "base.hpp"
#if SOUP_WINDOWS

#include "fwd.hpp"

#include "HandleBase.hpp"
#include "Range.hpp"
#include "UniquePtr.hpp"

namespace soup
{
	class Module
	{
	public:
		UniquePtr<HandleBase> h;
		Range range;

		Module() noexcept = default;
		Module(UniquePtr<HandleBase>&& h, Range&& range);
		Module(UniquePtr<HandleBase>&& h);
		Module(HANDLE h);
		Module(std::nullptr_t);
		Module(const char* name);

		[[nodiscard]] operator Range() const noexcept
		{
			return range;
		}

		[[nodiscard]] Pointer base() const noexcept
		{
			return range.base;
		}

		[[nodiscard]] size_t size() const noexcept
		{
			return range.size;
		}

		[[nodiscard]] Pointer getExport(const char* name) const noexcept;

		size_t externalRead(Pointer p, void* out, size_t size) const noexcept;

		template <typename T>
		[[nodiscard]] T externalRead(Pointer p) const noexcept
		{
			T val;
			externalRead(p, &val, sizeof(val));
			return val;
		}

		[[nodiscard]] Pointer externalScan(const Pattern& sig) const;

		[[nodiscard]] UniquePtr<AllocRaiiRemote> allocate(size_t size, DWORD type = MEM_COMMIT | MEM_RESERVE, DWORD protect = PAGE_EXECUTE_READWRITE) const;
		[[nodiscard]] UniquePtr<AllocRaiiRemote> copyInto(const void* data, size_t size) const;
		size_t externalWrite(Pointer p, const void* data, size_t size) const noexcept;

		template <typename T>
		size_t externalWrite(Pointer p, const T val)
		{
			return externalWrite(p, &val, sizeof(T));
		}

		template <typename T, size_t S>
		size_t externalWrite(Pointer p, const T(&data)[S])
		{
			return externalWrite(p, &data, S);
		}

		UniquePtr<HandleRaii> executeAsync(void* rip, uintptr_t rcx) const noexcept;
		void executeSync(void* rip, uintptr_t rcx) const noexcept;
	};
}
#endif
