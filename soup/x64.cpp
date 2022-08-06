#include "x64.hpp"

#include "macros.hpp"

namespace soup
{
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

	void x64::Operand::decode(bool rex, uint8_t size, uint8_t reg, bool x) noexcept
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

	std::string x64::Operand::toString() const
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

	std::string x64::Instruction::toString() const
	{
		std::string res = operation->name;
		if (operation->getOpr1Encoding() != ZO)
		{
			res.push_back(' ');
			res.append(operands[0].toString());
			if (operation->getOpr2Encoding() != ZO)
			{
				res.append(", ");
				res.append(operands[1].toString());
			}
		}
		return res;
	}

	// https://www.felixcloutier.com/x86/index.html

	x64::Instruction x64::disasm(const uint8_t*& code)
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
		for (const auto& op : operations)
		{
			if (op.matches(*code))
			{
				Instruction res{ &op };
				if (op.operand_encoding != ZO)
				{
					uint8_t operand_size;
					if (operand_size_override)
					{
						operand_size = 16;
					}
					else if (op.operand_size == 0)
					{
						operand_size = (default_operand_size ? 32 : 64);
					}
					else
					{
						operand_size = op.operand_size;
					}

					uint8_t opcode = *code;
					uint8_t opr_i = 0;
					bool modrm_read = false;
					uint8_t modrm;

					for (uint8_t opr_enc; opr_enc = ((op.operand_encoding >> (opr_i * BITS_PER_OPERAND)) & OPERAND_MASK), opr_enc != ZO; )
					{
						Operand& opr = res.operands[opr_i];
						if (opr_enc == O)
						{
							opr.decode(rex, operand_size, opcode & 0b111, rm_x);
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
									opr.deref_size = ((op.getOpr2Encoding() == ZO) ? operand_size : 1); // hiding pointer type when other operand makes it apparent
								}
							}
							else if (opr_enc == R)
							{
								opr.decode(rex, operand_size, (*code >> 3) & 0b111, reg_x);
							}
						}

						if (++opr_i == COUNT(res.operands))
						{
							break;
						}
					}
				}
				++code;
				return res;
			}
		}
		throw 0;
	}
}
