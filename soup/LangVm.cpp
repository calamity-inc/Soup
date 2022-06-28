#include "LangVm.hpp"

#include <typeinfo> // bad_cast

#include "BuiltinOp.hpp"
#include "ParseError.hpp"
#include "StringReader.hpp"

#define DEBUG_VM false

#if DEBUG_VM
#include <iostream>
#endif

namespace soup
{
	using Val = LangVm::Val;

	std::string& Val::getString() const
	{
		if (is_func)
		{
			throw std::bad_cast();
		}
		return Mixed::getString();
	}

	std::string& Val::getFunc() const
	{
		if (!is_func)
		{
			throw std::bad_cast();
		}
		return Mixed::getString();
	}

	bool LangVm::getNextOp(uint8_t& op)
	{
		while (r->u8(op))
		{
			if (op == OP_PUSH_STR)
			{
#if DEBUG_VM
				std::cout << "OP_PUSH_STR" << std::endl;
#endif
				std::string str;
				r->str_lp_u64_dyn(str);
				push(std::move(str));
			}
			else if (op == OP_PUSH_INT)
			{
#if DEBUG_VM
				std::cout << "OP_PUSH_INT" << std::endl;
#endif
				int64_t val;
				r->i64_dyn(val);
				push(val);
			}
			else if (op == OP_PUSH_VAR)
			{
#if DEBUG_VM
				std::cout << "OP_PUSH_VAR" << std::endl;
#endif
				uint8_t val;
				r->u8(val);
				push((uint64_t)val);
			}
			else if (op == OP_PUSH_FUN)
			{
#if DEBUG_VM
				std::cout << "OP_PUSH_FUN" << std::endl;
#endif
				std::string str;
				r->str_lp_u64_dyn(str);
				Val val = std::move(str);
				val.is_func = true;
				push(std::move(val));
			}
			else
			{
#if DEBUG_VM
				std::cout << "Op " << (int)op << std::endl;
#endif
				current_op = op; // this should be before this block if a builtin needs to pop
				return true;
			}
		}
		return false;
	}

	void LangVm::push(Val&& val)
	{
#if DEBUG_VM
		std::cout << "> Push " << val.getTypeName() << std::endl;
#endif
		stack.emplace(std::move(val));
	}

	Val LangVm::popRaw()
	{
		if (stack.empty())
		{
			std::string err = "Op ";
			err.append(std::to_string(current_op));
			err.append(" failed to pop a value because the stack is empty");
			throw ParseError(std::move(err));
		}
		Val val = std::move(stack.top());
#if DEBUG_VM
		std::cout << "> Pop " << val.getTypeName() << std::endl;
#endif
		stack.pop();
		return val;
	}

	Val LangVm::pop()
	{
		auto val = popRaw();
		if (val.isUInt())
		{
			if (auto e = vars.find((uint8_t)val.getUInt()); e != vars.end())
			{
				return e->second;
			}
			std::string err = "Op ";
			err.append(std::to_string(current_op));
			err.append(" expected a value, found undefined variable");
			throw ParseError(std::move(err));
		}
		return val;
	}

	Val& LangVm::popVarRef()
	{
		auto val = popRaw();
		if (!val.isUInt())
		{
			std::string err = "Op ";
			err.append(std::to_string(current_op));
			err.append(" expected a variable name, found ");
			err.append(val.getTypeName());
			throw ParseError(std::move(err));
		}
		auto idx = (uint8_t)val.getUInt();
		if (auto e = vars.find(idx); e != vars.end())
		{
			return e->second;
		}
		return vars.emplace(idx, Val{}).first->second;
	}

	std::string LangVm::popString()
	{
		auto val = pop();
		if (!val.isString())
		{
			std::string err = "Op ";
			err.append(std::to_string(current_op));
			err.append(" expected a function, found ");
			err.append(val.getTypeName());
			throw ParseError(std::move(err));
		}
		return std::move(val.getString());
	}

	StringReader LangVm::popFunc()
	{
		auto val = pop();
		if (!val.isFunc())
		{
			std::string err = "Op ";
			err.append(std::to_string(current_op));
			err.append(" expected a function, found ");
			err.append(val.getTypeName());
			throw ParseError(std::move(err));
		}
		return StringReader(std::move(val.getFunc()));
	}
}
