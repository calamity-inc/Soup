#include "irExpression.hpp"

#include "irFunction.hpp"

namespace soup
{
	std::string irExpression::toString(unsigned int depth) const noexcept
	{
		std::string str(depth * 4, ' ');
		switch (type)
		{
		case IR_CONST_BOOL: str.append("IR_CONST_BOOL"); break;
		case IR_CONST_I8: str.append("IR_CONST_I8"); break;
		case IR_CONST_I32: str.append("IR_CONST_I32"); break;
		case IR_CONST_I64: str.append("IR_CONST_I64"); break;
		case IR_CONST_PTR: str.append("IR_CONST_PTR"); break;
		case IR_LOCAL_GET: str.append("IR_LOCAL_GET"); break;
		case IR_LOCAL_SET: str.append("IR_LOCAL_SET"); break;
		case IR_CALL: str.append("IR_CALL"); break;
		case IR_RET: str.append("IR_RET"); break;
		case IR_WHILE: str.append("IR_WHILE"); break;
		case IR_ADD: str.append("IR_ADD"); break;
		case IR_SUB: str.append("IR_SUB"); break;
		case IR_MUL: str.append("IR_MUL"); break;
		case IR_SDIV: str.append("IR_SDIV"); break;
		case IR_UDIV: str.append("IR_UDIV"); break;
		case IR_EQUALS: str.append("IR_EQUALS"); break;
		case IR_NOTEQUALS: str.append("IR_NOTEQUALS"); break;
		case IR_LOAD_I8: str.append("IR_LOAD_I8"); break;
		case IR_STORE: str.append("IR_STORE"); break;
		case IR_I64_TO_PTR: str.append("IR_I64_TO_PTR"); break;
		case IR_I64_TO_I32: str.append("IR_I64_TO_I32"); break;
		case IR_I32_TO_I64_SX: str.append("IR_I32_TO_I64_SX"); break;
		case IR_I32_TO_I64_ZX: str.append("IR_I32_TO_I64_ZX"); break;
		case IR_I8_TO_I64_SX: str.append("IR_I8_TO_I64_SX"); break;
		case IR_I8_TO_I64_ZX: str.append("IR_I8_TO_I64_ZX"); break;
		}
		if (type == IR_CONST_BOOL) { str.append(": ").append(const_bool.value ? "true" : "false"); }
		if (type == IR_CONST_I8) { str.append(": ").append(std::to_string(const_i8.value)); }
		if (type == IR_CONST_I32) { str.append(": ").append(std::to_string(const_i32.value)); }
		if (type == IR_CONST_I64) { str.append(": ").append(std::to_string(const_i64.value)); }
		if (type == IR_CONST_PTR) { str.append(": ").append(std::to_string(const_ptr.value)); }
		if (type == IR_LOCAL_GET) { str.append(": ").append(std::to_string(local_get.index)); }
		if (type == IR_LOCAL_SET) { str.append(": ").append(std::to_string(local_set.index)); }
		if (type == IR_CALL) { str.append(": ").append(std::to_string(call.index)); }
		str.push_back('\n');
		++depth;
		for (const auto& child : children)
		{
			if (child)
			{
				str.append(child->toString(depth));
			}
			else
			{
				str.append(depth * 4, ' ');
				str.append("/!\\ nullptr /!\\\n");
			}
		}
		return str;
	}

	irType irExpression::getResultType(const irFunction& fn) const noexcept
	{
		if (type == IR_CONST_BOOL || type == IR_EQUALS || type == IR_NOTEQUALS)
		{
			return IR_BOOL;
		}
		if (type == IR_CONST_I8 || type == IR_LOAD_I8)
		{
			return IR_I8;
		}
		if (type == IR_CONST_I32 || type == IR_I64_TO_I32)
		{
			return IR_I32;
		}
		if (type == IR_CONST_PTR || type == IR_I64_TO_PTR)
		{
			return IR_PTR;
		}
		if (type == IR_LOCAL_GET)
		{
			return fn.getLocalType(local_get.index);
		}
		return IR_I64;
	}
}
