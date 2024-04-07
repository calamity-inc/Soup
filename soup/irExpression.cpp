#include "irExpression.hpp"

#include "irFunction.hpp"
#include "irVm.hpp"

NAMESPACE_SOUP
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
		case IR_IFELSE: str.append("IR_IFELSE"); break;
		case IR_WHILE: str.append("IR_WHILE"); break;
		case IR_DISCARD: str.append("IR_DISCARD"); break;
		case IR_ADD_I32: str.append("IR_ADD_I32"); break;
		case IR_ADD_I64: str.append("IR_ADD_I64"); break;
		case IR_ADD_PTR: str.append("IR_ADD_PTR"); break;
		case IR_SUB_I32: str.append("IR_SUB_I32"); break;
		case IR_SUB_I64: str.append("IR_SUB_I64"); break;
		case IR_MUL_I64: str.append("IR_MUL_I64"); break;
		case IR_SDIV_I64: str.append("IR_SDIV_I64"); break;
		case IR_UDIV_I64: str.append("IR_UDIV_I64"); break;
		case IR_SMOD_I64: str.append("IR_SMOD_I64"); break;
		case IR_UMOD_I64: str.append("IR_UMOD_I64"); break;
		case IR_EQUALS_I8: str.append("IR_EQUALS_I8"); break;
		case IR_EQUALS_I32: str.append("IR_EQUALS_I32"); break;
		case IR_EQUALS_I64: str.append("IR_EQUALS_I64"); break;
		case IR_NOTEQUALS_I8: str.append("IR_NOTEQUALS_I8"); break;
		case IR_NOTEQUALS_I32: str.append("IR_NOTEQUALS_I32"); break;
		case IR_NOTEQUALS_I64: str.append("IR_NOTEQUALS_I64"); break;
		case IR_LOAD_I8: str.append("IR_LOAD_I8"); break;
		case IR_STORE_I8: str.append("IR_STORE_I8"); break;
		case IR_STORE_I32: str.append("IR_STORE_I32"); break;
		case IR_STORE_I64: str.append("IR_STORE_I64"); break;
		case IR_I64_TO_PTR: str.append("IR_I64_TO_PTR"); break;
		case IR_I64_TO_I32: str.append("IR_I64_TO_I32"); break;
		case IR_I64_TO_I8: str.append("IR_I64_TO_I8"); break;
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
		if (type == IR_IFELSE) { str.append(": ").append(std::to_string(ifelse.ifinsns)); }
		if (type == IR_DISCARD) { str.append(": ").append(std::to_string(discard.count)); }
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

	bool irExpression::optimiseByConstantFolding()
	{
		bool any_change = false;
		if (isFoldableConstant())
		{
			if (type >= IR_ADD_I32 && type < IR_LOAD_I8)
			{
				irModule mod;
				std::string memory;
				irVm vm(memory);
				auto res = vm.execute(mod, *this);
				SOUP_ASSERT(res.size() == 1);
				switch (res.at(0).type)
				{
				case IR_BOOL:
					type = IR_CONST_BOOL;
					const_bool.value = res.at(0).value.b;
					break;

				case IR_I8:
					type = IR_CONST_I8;
					const_i8.value = res.at(0).value.i8;
					break;

				case IR_I32:
					type = IR_CONST_I32;
					const_i32.value = res.at(0).value.i32;
					break;

				case IR_I64:
					type = IR_CONST_I64;
					const_i64.value = res.at(0).value.i64;
					break;

				case IR_PTR:
					type = IR_CONST_PTR;
					const_ptr.value = res.at(0).value.ptr;
					break;
				}
				children.clear();
				any_change |= true;
			}
		}
		else
		{
			for (const auto& child : children)
			{
				any_change |= child->optimiseByConstantFolding();
			}
		}
		return any_change;
	}

	irType irExpression::getResultType(const irFunction& fn) const noexcept
	{
		if (type == IR_CONST_BOOL
			|| (type >= IR_EQUALS_I8 && type <= IR_NOTEQUALS_I64)
			)
		{
			return IR_BOOL;
		}
		if (type == IR_CONST_I8 || type == IR_LOAD_I8 || type == IR_I64_TO_I8)
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

	bool irExpression::isConstantZero() const noexcept
	{
		switch (type)
		{
		case IR_CONST_BOOL: return !const_bool.value;
		case IR_CONST_I8: return !const_i8.value;
		case IR_CONST_I32: return !const_i32.value;
		case IR_CONST_I64: return !const_i64.value;
		case IR_CONST_PTR: return !const_ptr.value;
		default: SOUP_ASSERT(type > IR_CONST_PTR); return false;
		}
	}

	bool irExpression::isNegativeCompareToConstantZero() const noexcept
	{
		if (type >= IR_NOTEQUALS_I8 && type <= IR_NOTEQUALS_I64)
		{
			return children.at(0)->isConstantZero() || children.at(1)->isConstantZero();
		}
		return false;
	}

	bool irExpression::isFoldableConstant() const noexcept
	{
		if (type <= IR_CONST_PTR)
		{
			return true;
		}
		if (type >= IR_ADD_I32 && type < IR_LOAD_I8)
		{
			for (const auto& child : children)
			{
				if (!child->isFoldableConstant())
				{
					return false;
				}
			}
			return true;
		}
		return false;
	}

	UniquePtr<irExpression> irExpression::clone() const
	{
		auto insn = soup::make_unique<irExpression>(this->type);
		insn->union_value = this->union_value;
		insn->children.reserve(children.size());
		for (const auto& child : children)
		{
			insn->children.emplace_back(child->clone());
		}
		return insn;
	}

	UniquePtr<irExpression> irExpression::inverted() const
	{
		if (type == IR_CONST_BOOL)
		{
			auto insn = soup::make_unique<irExpression>(IR_CONST_BOOL);
			insn->const_bool.value = !this->const_bool.value;
			return insn;
		}
		if (type >= IR_EQUALS_I8 && type <= IR_EQUALS_I64)
		{
			auto insn = clone();
			insn->type = static_cast<irExpressionType>(insn->type + (IR_NOTEQUALS_I8 - IR_EQUALS_I8));
			return insn;
		}
		if (type >= IR_NOTEQUALS_I8 && type <= IR_NOTEQUALS_I64)
		{
			auto insn = clone();
			insn->type = static_cast<irExpressionType>(insn->type + (IR_EQUALS_I8 - IR_NOTEQUALS_I8));
			return insn;
		}
		SOUP_ASSERT_UNREACHABLE;
	}
}
