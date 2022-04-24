#pragma once

#include "fwd.hpp"

#include "Pointerlike.hpp"

#include <vector>

namespace soup
{
	class Pointer : public Pointerlike<Pointer>
	{
	private:
		void* ptr;

	public:
		Pointer(void* ptr = nullptr) noexcept
			: ptr(ptr)
		{
		}

		Pointer(uintptr_t ptr) noexcept
			: ptr((void*)ptr)
		{
		}

		explicit operator bool() const
		{
			return ptr != nullptr;
		}

		friend bool operator==(Pointer a, Pointer b) noexcept
		{
			return a.ptr == b.ptr;
		}

		friend bool operator!=(Pointer a, Pointer b) noexcept
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
		inline Pointer ripT() const noexcept
		{
			return add(as<T&>()).add(sizeof(T));
		}

		Pointer rip() const noexcept;

#if SOUP_WINDOWS
		Pointer externalRip(const Module& mod) const noexcept;

		Pointer rva(const Module& mod) const noexcept;

		[[nodiscard]] std::vector<Pointer> getJumps() const noexcept;
		[[nodiscard]] Pointer followJumps() const noexcept;
#endif
	};
}
