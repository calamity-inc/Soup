#include "irExpression.hpp"

namespace soup
{
	std::string irExpression::toString(unsigned int depth) const noexcept
	{
		std::string str(depth * 4, ' ');
		switch (type)
		{
		case IR_CONST_I64: str.append("IR_CONST_I64"); break;
		case IR_LOCAL: str.append("IR_LOCAL"); break;
		case IR_CALL: str.append("IR_CALL"); break;
		case IR_ADD: str.append("IR_ADD"); break;
		case IR_SUB: str.append("IR_SUB"); break;
		case IR_MUL: str.append("IR_MUL"); break;
		case IR_SDIV: str.append("IR_SDIV"); break;
		case IR_UDIV: str.append("IR_UDIV"); break;
		case IR_RET: str.append("IR_RET"); break;
		}
		if (type == IR_CONST_I64)
		{
			str.append(": ");
			str.append(std::to_string(constant_value));
		}
		else if (type == IR_LOCAL || type == IR_CALL)
		{
			str.append(": ");
			str.append(std::to_string(index));
		}
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
}
