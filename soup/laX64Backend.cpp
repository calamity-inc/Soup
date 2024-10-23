#include "laX64Backend.hpp"

#include "string.hpp"
#include "StringWriter.hpp"
#include "utility.hpp" // SOUP_MOVE_RETURN

NAMESPACE_SOUP
{
	std::string laX64Backend::compileFunction(const irModule& m, const irFunction& fn)
	{
		StringWriter w;
		for (auto i = fn.insns.begin(); i != fn.insns.end(); ++i)
		{
			irExpression& insn = **i;
			int ret = compileExpression(m, w, insn, nullptr);
			if (insn.type == IR_RET)
			{
				break;
			}
			discard(w, ret);
		}
		if (true)
		{
			// It's very likely we generated `push r10` followed directly `pop r10` somewhere, this is a no-op and can be erased.
			string::replaceAll(w.data, std::string("\x41\x52\x41\x5a", 4), {});
		}
		SOUP_MOVE_RETURN(w.data);
	}

	int laX64Backend::compileExpression(const irModule& m, StringWriter& w, const irExpression& e, const irExpression* outgoing)
	{
		uint8_t b;
#if false // stack-based approach
		switch (e.type)
		{
		case IR_LOCAL_GET:
			// Note: This is for the Windows ABI. Linux places call arguments in different registers.
			switch (e.local_get.index)
			{
			case 0: b = 0x51; w.u8(b); break; // push rcx
			case 1: b = 0x52; w.u8(b); break; // push rdx
			case 2: b = 0x41; w.u8(b); b = 0x50; w.u8(b); break; // push r8
			case 3: b = 0x41; w.u8(b); b = 0x51; w.u8(b); break; // push r9
			// The rest would be on the stack...
			default: SOUP_ASSERT_UNREACHABLE;
			}
			return 1;

		case IR_ADD_I64:
			SOUP_ASSERT(e.children.size() == 2);
			SOUP_ASSERT(compileExpression(m, w, *e.children[0]) == 1);
			SOUP_ASSERT(compileExpression(m, w, *e.children[1]) == 1);
			b = 0x41; w.u8(b); b = 0x5b; w.u8(b); // pop r11
			b = 0x41; w.u8(b); b = 0x5a; w.u8(b); // pop r10
			b = 0x4d; w.u8(b); b = 0x01; w.u8(b); b = 0xda; w.u8(b); // add r10, r11
			b = 0x41; w.u8(b); b = 0x52; w.u8(b); // push r10
			return 1;

		case IR_SUB_I64:
			SOUP_ASSERT(e.children.size() == 2);
			SOUP_ASSERT(compileExpression(m, w, *e.children[0]) == 1);
			SOUP_ASSERT(compileExpression(m, w, *e.children[1]) == 1);
			b = 0x41; w.u8(b); b = 0x5b; w.u8(b); // pop r11
			b = 0x41; w.u8(b); b = 0x5a; w.u8(b); // pop r10
			b = 0x4d; w.u8(b); b = 0x29; w.u8(b); b = 0xda; w.u8(b); // sub r10, r11
			b = 0x41; w.u8(b); b = 0x52; w.u8(b); // push r10
			return 1;

		case IR_RET:
			SOUP_ASSERT(e.children.size() == 1);
			SOUP_ASSERT(compileExpression(m, w, *e.children[0]) == 1);
			b = 0x58; w.u8(b); // pop rax
			b = 0xc3; w.u8(b); // ret
			return 1;

		default:
			SOUP_ASSERT_UNREACHABLE;
		}
#else
		switch (e.type)
		{
		case IR_LOCAL_GET:
			return 0;

		case IR_ADD_I64:
			{
				SOUP_ASSERT(e.children.size() == 2);
				x64Register a1_reg = (compileExpression(m, w, *e.children[0], &e) == 0) ? getIncomingRegister(*e.children[0]) : R10;
				x64Register a2_reg = (compileExpression(m, w, *e.children[1], &e) == 0) ? getIncomingRegister(*e.children[1]) : R11;
				x64Register res_reg = (outgoing && outgoing->type == IR_RET) ? RA : R10;
				if (a2_reg == R11)
				{
					b = 0x41; w.u8(b); b = 0x5b; w.u8(b); // pop r11
				}
				if (a1_reg == R10)
				{
					b = 0x41; w.u8(b); b = 0x5a; w.u8(b); // pop r10
				}

				x64Instruction insn{};
				insn.setOperationFromOpcode(0x8D); // lea
				insn.operands[0].reg = res_reg;
				insn.operands[0].access_type = ACCESS_64;
				insn.operands[1].reg = OFF;
				insn.operands[1].off.a = a1_reg;
				insn.operands[1].off.b = a2_reg;
				auto bytecode = insn.toBytecode();
				w.str(bytecode.size(), bytecode.data());

				if (res_reg == R10)
				{
					b = 0x41; w.u8(b); b = 0x52; w.u8(b); // push r10
					return 1;
				}
				return 0;
			}

		case IR_SUB_I64:
			{
				SOUP_ASSERT(e.children.size() == 2);
				x64Register a1_reg = (compileExpression(m, w, *e.children[0], &e) == 0) ? getIncomingRegister(*e.children[0]) : R10;
				x64Register a2_reg = (compileExpression(m, w, *e.children[1], &e) == 0) ? getIncomingRegister(*e.children[1]) : R11;
				x64Register res_reg = (outgoing && outgoing->type == IR_RET) ? RA : R10;
				if (a2_reg == R11)
				{
					b = 0x41; w.u8(b); b = 0x5b; w.u8(b); // pop r11
				}
				if (a1_reg == R10)
				{
					b = 0x41; w.u8(b); b = 0x5a; w.u8(b); // pop r10
				}

				x64Instruction mov_insn{};
				mov_insn.setOperationFromOpcode(0x89);
				mov_insn.operands[0].reg = res_reg;
				mov_insn.operands[0].access_type = ACCESS_64;
				mov_insn.operands[1].reg = a1_reg;
				mov_insn.operands[1].access_type = ACCESS_64;
				auto bytecode = mov_insn.toBytecode();
				w.str(bytecode.size(), bytecode.data());

				x64Instruction sub_insn{};
				sub_insn.setOperationFromOpcode(0x29);
				sub_insn.operands[0].reg = res_reg;
				sub_insn.operands[0].access_type = ACCESS_64;
				sub_insn.operands[1].reg = a2_reg;
				sub_insn.operands[1].access_type = ACCESS_64;
				bytecode = sub_insn.toBytecode();
				w.str(bytecode.size(), bytecode.data());

				if (res_reg == R10)
				{
					b = 0x41; w.u8(b); b = 0x52; w.u8(b); // push r10
					return 1;
				}
				return 0;
			}

		case IR_RET:
			SOUP_ASSERT(e.children.size() == 1);
			SOUP_ASSERT(compileExpression(m, w, *e.children[0], &e) == 0);
			b = 0xc3; w.u8(b); // ret
			return 0;

		default:
			SOUP_ASSERT_UNREACHABLE;
		}
#endif
	}

	void laX64Backend::discard(StringWriter& w, int nres)
	{
		uint8_t b;
		while (nres--)
		{
			b = 0x41; w.u8(b); b = 0x5a; w.u8(b); // pop r10
		}
	}

	x64Register laX64Backend::getIncomingRegister(const irExpression& e)
	{
		switch (e.type)
		{
		case IR_LOCAL_GET:
			// Note: This is for the Windows ABI. Linux places call arguments in different registers.
			switch (e.local_get.index)
			{
			case 0: return RC;
			case 1: return RD;
			case 2: return R8;
			case 3: return R9;
			// The rest would be on the stack...
			default: SOUP_ASSERT_UNREACHABLE;
			}

		default:
			break;
		}
		return R10;
	}
}
