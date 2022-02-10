#pragma once

#include "base.hpp"
#if SOUP_WINDOWS

#include <memory>

#include "fwddecl.hpp"

#include "handle_base.hpp"
#include "range.hpp"

namespace soup
{
	class module
	{
	public:
		std::unique_ptr<handle_base> h;
		range m_range;

		module() noexcept = default;
		module(std::unique_ptr<handle_base>&& h, range&& range);
		module(std::unique_ptr<handle_base>&& h);
		module(HANDLE h);
		module(std::nullptr_t);
		module(const char* name);

		[[nodiscard]] operator range() const noexcept;

		[[nodiscard]] pointer base() const noexcept;
		[[nodiscard]] size_t size() const noexcept;

		[[nodiscard]] pointer getExport(const char* name) const noexcept;

		size_t externalRead(pointer p, void* out, size_t size) const noexcept;

		template <typename T>
		[[nodiscard]] T externalRead(pointer p) const noexcept
		{
			T val;
			externalRead(p, &val, sizeof(val));
			return val;
		}

		[[nodiscard]] pointer externalScan(const pattern& sig) const;

		[[nodiscard]] std::unique_ptr<alloc_raii_remote> allocate(size_t size, DWORD type = MEM_COMMIT | MEM_RESERVE, DWORD protect = PAGE_EXECUTE_READWRITE) const;
		[[nodiscard]] std::unique_ptr<alloc_raii_remote> copyInto(const void* data, size_t size) const;
		size_t externalWrite(pointer p, const void* data, size_t size) const noexcept;
		
		template <typename T>
		size_t externalWrite(pointer p, const T val)
		{
			return externalWrite(p, &val, sizeof(T));
		}
		
		template <typename T, size_t S>
		size_t externalWrite(pointer p, const T(&data)[S])
		{
			return externalWrite(p, &data, S);
		}

		std::unique_ptr<handle_raii> executeAsync(void* rip, uintptr_t rcx) const noexcept;
		void executeSync(void* rip, uintptr_t rcx) const noexcept;
	};
}
#endif
