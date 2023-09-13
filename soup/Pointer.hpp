#pragma once

#include "fwd.hpp"

#include "Pointerlike.hpp"

#include <vector>

namespace soup
{
	class Pointer : public Pointerlike<Pointer>
	{
	private:
		union
		{
			void* ptr;
			uintptr_t uptr;
		};

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

		friend bool operator>(Pointer a, Pointer b) noexcept
		{
			return a.uptr >= b.uptr;
		}

		friend bool operator<(Pointer a, Pointer b) noexcept
		{
			return a.uptr <= b.uptr;
		}
		
		friend bool operator>=(Pointer a, Pointer b) noexcept
		{
			return a.uptr >= b.uptr;
		}

		friend bool operator<=(Pointer a, Pointer b) noexcept
		{
			return a.uptr <= b.uptr;
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

		Pointer rip() const noexcept
		{
			return ripT<int32_t>();
		}

#if SOUP_WINDOWS
		Pointer externalRip(const Module& mod) const noexcept;

		[[nodiscard]] bool isInModule() const noexcept;
		[[nodiscard]] bool isInModule(const Module& mod) const noexcept;
		[[nodiscard]] bool isInRange(const Range& range) const noexcept;

		Pointer rva() const noexcept;
		Pointer rva(const Module& mod) const noexcept;

		[[nodiscard]] std::vector<Pointer> getJumps() const noexcept;
		[[nodiscard]] Pointer followJumps() const noexcept;
#endif
	};
}
