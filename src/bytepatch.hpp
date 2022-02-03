#pragma once

#include "stdint.hpp"

namespace soup
{
	class bytepatch
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
		explicit constexpr bytepatch() = default;
		~bytepatch();

		bytepatch(bytepatch&& b);
		void operator =(bytepatch&& b);
	private:
		void patchFrom(bytepatch&& b);

	public:
		[[nodiscard]] bool isPatched() const;
		void dispose();

	private:
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
