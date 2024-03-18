#pragma once

#include <cstdint>
#include <cstddef> // size_t

namespace soup
{
	class Bytepatch
	{
	public:
		enum Instructions : uint8_t
		{
			NOP = 0x90,
			RET = 0xC3,
			JMP1 = 0xEB,
		};

	private:
		uint8_t* og_area = nullptr;
		size_t size = 0;
		uint8_t* area = nullptr;

	public:
		explicit constexpr Bytepatch() = default;
		~Bytepatch() noexcept;

		Bytepatch(Bytepatch&& b) noexcept;
		void operator =(Bytepatch&& b) noexcept;
	private:
		void patchFrom(Bytepatch&& b) noexcept;

	public:
		[[nodiscard]] bool isPatched() const noexcept;
	private:
		void forget() noexcept;
		void store() noexcept;
		void store(uint8_t* area, size_t size) noexcept;
	public:
		void initPatch(uint8_t* area, const uint8_t* patch, size_t size) noexcept;
		bool initPatchNOP(uint8_t* area, size_t size) noexcept;
		void initPatchZero(uint8_t* area, size_t size) noexcept;
		void initPatchZero() noexcept;
		void restore() noexcept;
		void dispose() noexcept;
	};
}
