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
		bool deref = false;

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
		}

		void setDeref(bool address_size_override) noexcept
		{
			access_type = address_size_override ? ACCESS_32 : ACCESS_64;
			deref = true;
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
			if (deref)
			{
				name.insert(0, 1, '[');
				name.push_back(']');
			}
			return name;
		}
	};

	enum InsMode : uint8_t
	{
		ZO = 0,
		MR,
		RM,
	};

	struct InsInfo
	{
		const char* name;
		uint8_t opcode;
		InsMode mode;
		uint8_t operand_size = 0;
	};

	// https://www.felixcloutier.com/x86/index.html

	static InsInfo instructions[] = {
		{ "mov", 0x88, MR, 8 },
		{ "mov", 0x89, MR },
		{ "mov", 0x8A, RM, 8 },
		{ "mov", 0x8B, RM },
		{ "ret", 0xC3, ZO },
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
			if (ins.opcode == *code)
			{
				++code;
				std::string res = ins.name;
				if (ins.mode != ZO)
				{
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
					InsOperand left, right;
					bool direct = ((*code >> 6) == 0b11);
					left.decode(rex, operand_size, (*code >> 3) & 0b111, reg_x);
					right.decode(rex, operand_size, *code & 0b111, rm_x);
					if (ins.mode == MR)
					{
						if (!direct)
						{
							left.setDeref(address_size_override);
						}
					}
					else if (ins.mode == RM)
					{
						if (!direct)
						{
							right.setDeref(address_size_override);
						}
					}

					res.push_back(' ');
					res.append(left.toString());
					res.append(", ");
					res.append(right.toString());
					++code;
				}
				return res;
			}
		}
		throw 0;
	}
}
