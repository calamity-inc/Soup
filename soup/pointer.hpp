#pragma once

#include "fwd.hpp"

#include "pointerlike.hpp"

#include <vector>

namespace soup
{
	class pointer : public pointerlike<pointer>
	{
	private:
		void* ptr;

	public:
		pointer(void* ptr = nullptr) noexcept
			: ptr(ptr)
		{
		}

		pointer(uintptr_t ptr) noexcept
			: ptr((void*)ptr)
		{
		}

		explicit operator bool() const
		{
			return ptr != nullptr;
		}

		friend bool operator==(pointer a, pointer b) noexcept
		{
			return a.ptr == b.ptr;
		}

		friend bool operator!=(pointer a, pointer b) noexcept
		{
			return a.ptr != b.ptr;
		}

		void operator=(uintptr_t ptr) noexcept
		{
			this->ptr = reinterpret_cast<void*>(ptr);
		}

		void* addr() const noexcept
		{
			return ptr;
		}

		[[nodiscard]] uintptr_t offset() const noexcept
		{
			return (uintptr_t)ptr;
		}

		template <typename T>
		inline pointer ripT() const noexcept
		{
			return add(as<T&>()).add(sizeof(T));
		}

		pointer rip() const noexcept;

#if SOUP_WINDOWS
		pointer externalRip(const module& mod) const noexcept;

		pointer rva(const module& mod) const noexcept;

		[[nodiscard]] std::vector<pointer> getJumps() const noexcept;
		[[nodiscard]] pointer followJumps() const noexcept;
#endif
	};
}
