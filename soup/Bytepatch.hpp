#pragma once

#include "base.hpp"

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
		~Bytepatch();

		Bytepatch(Bytepatch&& b);
		void operator =(Bytepatch&& b);
	private:
		void patchFrom(Bytepatch&& b);

	public:
		[[nodiscard]] bool isPatched() const;
	private:
		void forget();
		void store();
		void store(uint8_t* area, size_t size);
	public:
		void initPatch(uint8_t* area, const uint8_t* patch, size_t size);
		bool initPatchNOP(uint8_t* area, size_t size);
		void initPatchZero(uint8_t* area, size_t size);
		void initPatchZero();
		void restore();
	};
}
