#pragma once

#include <cstdint>
#include <string>

namespace soup
{
	struct x64
	{
		enum Register : uint8_t
		{
			RA = 0,
			RC,
			RB,
			RD,
			SP,
			BP,
			SI,
			DI,
			R8,
			R9,
			R10,
			R11,
			R12,
			R13,
			R14,
			R15,
			IP,

			IMM,
		};

		enum RegisterAccessType : uint8_t
		{
			ACCESS_64 = 64,
			ACCESS_32 = 32,
			ACCESS_16 = 16,
			ACCESS_8 = 8,
			ACCESS_8_H = 0,
		};

#pragma pack(push, 1)
		struct Operand
		{
			Register reg;
			union
			{
				struct // register
				{
					/* 0 */ RegisterAccessType access_type;
					/* 1 */ uint8_t deref_size;
					/* 2 */ int32_t deref_offset;
					/* 6 */
				};
				struct // immediate
				{
					/* 0 */ uint64_t val;
					/* 8 */
				};
			};

			Operand()
				: val(0)
			{
			}

			void decode(bool rex, uint8_t size, uint8_t reg, bool x) noexcept;

			[[nodiscard]] std::string toString() const;
		};
#pragma pack(pop)

		enum OperandEncoding : uint8_t
		{
			ZO = 0,

			O = 0b001,
			M = 0b010,
			R = 0b011,
			I = 0b100,
			A = 0b101,

			OPERAND_MASK = 0b111,
			BITS_PER_OPERAND = 3,

			MR = M | (R << BITS_PER_OPERAND),
			RM = R | (M << BITS_PER_OPERAND),
			OI = O | (I << BITS_PER_OPERAND),
			MI = M | (I << BITS_PER_OPERAND),
			AI = A | (I << BITS_PER_OPERAND),
		};

		struct Operation
		{
			const char* const name;
			const uint8_t opcode;
			const OperandEncoding operand_encoding;
			const uint8_t operand_size;
			const uint8_t distinguish;

			Operation(const char* name, uint8_t opcode, OperandEncoding operand_encoding)
				: Operation(name, opcode, operand_encoding, 0, 0)
			{
			}

			Operation(const char* name, uint8_t opcode, OperandEncoding operand_encoding, uint8_t operand_size)
				: Operation(name, opcode, operand_encoding, operand_size, 0)
			{
			}

			Operation(const char* name, uint8_t opcode, OperandEncoding operand_encoding, uint8_t operand_size, uint8_t distinguish)
				: name(name), opcode(opcode), operand_encoding(operand_encoding), operand_size(operand_size), distinguish(distinguish)
			{
			}

			[[nodiscard]] bool matches(const uint8_t* code) const noexcept
			{
				uint8_t b = *code;
				if (getOpr1Encoding() == O)
				{
					b &= 0b11111000;
				}
				return opcode == b
					&& (distinguish == 0
						|| ((code[1] >> 3) & 0b111) == distinguish
						)
					;
			}

			[[nodiscard]] OperandEncoding getOpr1Encoding() const noexcept
			{
				return (OperandEncoding)(operand_encoding & OPERAND_MASK);
			}

			[[nodiscard]] OperandEncoding getOpr2Encoding() const noexcept
			{
				return (OperandEncoding)((operand_encoding >> BITS_PER_OPERAND) & OPERAND_MASK);
			}

			[[nodiscard]] uint8_t getNumOperands() const noexcept
			{
				if (getOpr1Encoding() == ZO)
				{
					return 0;
				}
				if (getOpr2Encoding() == ZO)
				{
					return 1;
				}
				return 2;
			}

			[[nodiscard]] uint8_t getNumModrmOperands() const noexcept
			{
				uint8_t count = 0;
				if (getOpr1Encoding() == M || getOpr1Encoding() == R)
				{
					++count;
				}
				if (getOpr2Encoding() == M || getOpr2Encoding() == R)
				{
					++count;
				}
				return count;
			}
		};

		inline static Operation operations[] = {
			{ "mov", 0x88, MR, 8 },
			{ "mov", 0x89, MR },
			{ "mov", 0x8A, RM, 8 },
			{ "mov", 0x8B, RM },
			{ "mov", 0xB0, OI, 8 },
			{ "mov", 0xB8, OI },
			{ "mov", 0xC6, MI, 8 },
			{ "mov", 0xC7, MI, 32 },
			{ "ret", 0xC3, ZO },
			{ "push", 0x50, O, 64 },
			{ "push", 0xFF, M, 64 },
			{ "sub", 0x83, MI, 8, 5 },
			{ "cmp", 0x80, MI, 8, 7 },
			{ "cmp", 0x83, MI, 8, 7 },
			{ "cmp", 0x3C, AI, 8 },
			{ "cmp", 0x3D, AI, 32 },
			{ "cmp", 0x38, MR, 8 },
			{ "cmp", 0x39, MR },
		};

		struct Instruction
		{
			const Operation* operation = nullptr;
			Operand operands[2];

			[[nodiscard]] bool isValid() const noexcept
			{
				return operation != nullptr;
			}

			[[nodiscard]] uint8_t getNumOperands() const noexcept
			{
				return operation->getNumOperands();
			}

			[[nodiscard]] std::string toString() const;
		};

		[[nodiscard]] static Instruction disasm(const uint8_t*& code);
	};
}
