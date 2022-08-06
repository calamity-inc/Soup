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
				struct
				{
					RegisterAccessType access_type;
					uint8_t deref_size;
					uint8_t deref_offset;
				}; // 3 bytes
				uint64_t val = 0;
			};

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

			OPERAND_MASK = 0b111,
			BITS_PER_OPERAND = 3,

			MR = M | (R << BITS_PER_OPERAND),
			RM = R | (M << BITS_PER_OPERAND),
			OI = O | (I << BITS_PER_OPERAND),
			MI = M | (I << BITS_PER_OPERAND),
		};

		struct Operation
		{
			const char* const name;
			const uint8_t opcode;
			const OperandEncoding operand_encoding;
			const uint8_t operand_size;

			Operation(const char* name, uint8_t opcode, OperandEncoding operand_encoding)
				: name(name), opcode(opcode), operand_encoding(operand_encoding), operand_size(0)
			{
			}
			
			Operation(const char* name, uint8_t opcode, OperandEncoding operand_encoding, uint8_t operand_size)
				: name(name), opcode(opcode), operand_encoding(operand_encoding), operand_size(operand_size)
			{
			}

			[[nodiscard]] bool matches(uint8_t code) const noexcept
			{
				if (getOpr1Encoding() == O)
				{
					code &= 0b11111000;
				}
				return opcode == code;
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
		};

		inline static Operation operations[] = {
			{ "mov", 0x88, MR, 8 },
			{ "mov", 0x89, MR },
			{ "mov", 0x8A, RM, 8 },
			{ "mov", 0x8B, RM },
			{ "mov", 0xB0, OI, 8 },
			{ "mov", 0xB8, OI },
			{ "mov", 0xC7, MI, 32 },
			{ "ret", 0xC3, ZO },
			{ "push", 0x50, O, 64 },
			{ "push", 0xFF, M, 64 },
			{ "sub", 0x83, MI, 8 },
		};

		struct Instruction
		{
			const Operation* operation;
			Operand operands[2];

			[[nodiscard]] uint8_t getNumOperands() const noexcept
			{
				return operation->getNumOperands();
			}

			[[nodiscard]] std::string toString() const;
		};

		[[nodiscard]] static Instruction disasm(const uint8_t*& code);
	};
}
