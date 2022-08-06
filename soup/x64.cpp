#include "x64.hpp"

namespace soup
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
	};

	enum RegisterAccessType : uint8_t
	{
		ACCESS_64 = 64,
		ACCESS_32 = 32,
		ACCESS_16 = 16,
		ACCESS_8 = 8,
		ACCESS_8_H = 0,
	};

	const char* reg_names[16] = {
		"a",
		"c",
		"d",
		"b",
		"sp",
		"bp",
		"si",
		"di",
		"8",
		"9",
		"10",
		"11",
		"12",
		"13",
		"14",
		"15",
	};

	struct InsOperand
	{
		Register reg;
		RegisterAccessType access_type;
		uint8_t deref_size = 0;

		void decode(bool rex, uint8_t size, uint8_t reg, bool x) noexcept
		{
			reg |= (x << 3);

			this->reg = (Register)reg;
			access_type = (RegisterAccessType)size;

			if (reg >= SP && reg <= DI
				&& !rex
				)
			{
				this->reg = (Register)(this->reg - 4);
				access_type = ACCESS_8_H;
			}

			deref_size = 0;
		}

		[[nodiscard]] std::string toString() const
		{
			std::string name{};
			if (reg < R8)
			{
				name = reg_names[reg];
				if (access_type == ACCESS_8_H)
				{
					name.push_back('h');
				}
				else if (access_type == ACCESS_8)
				{
					name.push_back('l');
				}
				else // >= 16-bit
				{
					if (reg < SP)
					{
						name.push_back('x');
					}
					if (access_type == ACCESS_32)
					{
						name.insert(0, 1, 'e');
					}
					else if (access_type == ACCESS_64)
					{
						name.insert(0, 1, 'r');
					}
				}
			}
			else // >= R8
			{
				name.push_back('r');
				name.append(reg_names[reg]);
				if (access_type == ACCESS_32)
				{
					name.push_back('d');
				}
				else if (access_type == ACCESS_16)
				{
					name.push_back('w');
				}
				else if (access_type == ACCESS_8)
				{
					name.push_back('l');
				}
			}
			if (deref_size != 0)
			{
				name.insert(0, 1, '[');
				if (deref_size == 8)
				{
					name.insert(0, "byte ptr ");
				}
				else if (deref_size == 16)
				{
					name.insert(0, "word ptr ");
				}
				else if (deref_size == 32)
				{
					name.insert(0, "dword ptr ");
				}
				else if (deref_size == 64)
				{
					name.insert(0, "qword ptr ");
				}
				name.push_back(']');
			}
			return name;
		}
	};

	enum InsOperandEncoding : uint8_t
	{
		ZO = 0,

		O = 0b01,
		M = 0b10,
		R = 0b11,

		OPERAND_MASK = 0b11,
		BITS_PER_OPERAND = 2,

		MR = M | (R << BITS_PER_OPERAND),
		RM = R | (M << BITS_PER_OPERAND),
	};

	struct InsInfo
	{
		const char* name;
		uint8_t opcode;
		InsOperandEncoding operand_encoding;
		uint8_t operand_size = 0;

		[[nodiscard]] bool matches(uint8_t code) const noexcept
		{
			if (getOpr1Encoding() == O)
			{
				code &= 0b11111000;
			}
			return opcode == code;
		}

		[[nodiscard]] InsOperandEncoding getOpr1Encoding() const noexcept
		{
			return (InsOperandEncoding)(operand_encoding & OPERAND_MASK);
		}

		[[nodiscard]] InsOperandEncoding getOpr2Encoding() const noexcept
		{
			return (InsOperandEncoding)((operand_encoding >> BITS_PER_OPERAND) & OPERAND_MASK);
		}
	};

	// https://www.felixcloutier.com/x86/index.html

	static InsInfo instructions[] = {
		{ "mov", 0x88, MR, 8 },
		{ "mov", 0x89, MR },
		{ "mov", 0x8A, RM, 8 },
		{ "mov", 0x8B, RM },
		{ "ret", 0xC3, ZO },
		{ "push", 0x50, O, 64 },
		{ "push", 0xFF, M, 64 },
	};

	std::string x64::disasm(const uint8_t*& code)
	{
		bool operand_size_override = false;
		bool address_size_override = false;
		bool rex = false;
		bool default_operand_size = true;
		bool reg_x = false;
		bool rm_x = false;

		// "Mandatory" prefix
		if (*code == 0x67)
		{
			address_size_override = true;
			++code;
		}
		if (*code == 0x66)
		{
			operand_size_override = true;
			++code;
		}

		// REX prefix
		if ((*code >> 4) == 0b0100)
		{
			rex = true;
			if ((*code >> 3) & 1)
			{
				default_operand_size = false;
			}
			reg_x = ((*code >> 2) & 1);
			rm_x = (*code & 1);
			++code;
		}

		// Opcode
		for (const auto& ins : instructions)
		{
			if (ins.matches(*code))
			{
				std::string res = ins.name;
				if (ins.operand_encoding != ZO)
				{
					res.push_back(' ');

					uint8_t operand_size;
					if (operand_size_override)
					{
						operand_size = 16;
					}
					else if (ins.operand_size == 0)
					{
						operand_size = (default_operand_size ? 32 : 64);
					}
					else
					{
						operand_size = ins.operand_size;
					}

					uint8_t opcode = *code;
					InsOperand opr;
					uint8_t opr_offset = 0;
					bool opr_cont = false;
					bool modrm_read = false;
					uint8_t modrm;

					for (uint8_t opr_enc; opr_enc = ((ins.operand_encoding >> opr_offset) & OPERAND_MASK), opr_enc != ZO; opr_offset += BITS_PER_OPERAND)
					{
						if (opr_cont)
						{
							res.append(", ");
						}
						if (opr_enc == O)
						{
							opr.decode(rex, operand_size, opcode & 0b111, rm_x);
							res.append(opr.toString());
						}
						else if (opr_enc == M || opr_enc == R)
						{
							const bool left = !modrm_read;
							if (left)
							{
								modrm_read = true;
								modrm = *++code;
							}
							const bool direct = ((modrm >> 6) == 0b11);
							if (opr_enc == M)
							{
								opr.decode(rex, operand_size, modrm & 0b111, rm_x);
								if (!direct)
								{
									opr.access_type = (address_size_override ? ACCESS_32 : ACCESS_64);
									opr.deref_size = ((ins.getOpr2Encoding() == ZO) ? operand_size : 1); // hiding pointer type when other operand makes it apparent
								}
								res.append(opr.toString());
							}
							else if (opr_enc == R)
							{
								opr.decode(rex, operand_size, (*code >> 3) & 0b111, reg_x);
								res.append(opr.toString());
							}
						}
						opr_cont = true;
					}
				}
				++code;
				return res;
			}
		}
		throw 0;
	}
}
