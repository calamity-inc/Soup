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
	LangVm::LangVm(Reader* r, std::stack<std::shared_ptr<Mixed>>&& stack) noexcept
		: r(r), stack(std::move(stack))
#if DEBUG_VM
		, current_op(OP_PUSH_STR)
#endif
	{
#if DEBUG_VM
		std::cout << "--- Create Scope ---" << std::endl;
#endif
	}

	LangVm::~LangVm() noexcept
	{
#if DEBUG_VM
		if (current_op == OP_PUSH_INT)
		{
			std::cout << "--- Delete Scope ---" << std::endl;
		}
#endif
	}

	bool LangVm::getNextOp(uint8_t& op)
	{
		while (r->u8(op))
		{
			current_op = op;
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
				push(std::make_shared<Mixed>(val));
			}
			else if (op == OP_PUSH_UINT)
			{
#if DEBUG_VM
				std::cout << "OP_PUSH_UINT" << std::endl;
#endif
				uint64_t val;
				r->u64_dyn(val);
				push(std::make_shared<Mixed>(val));
			}
			else if (op == OP_PUSH_FUN)
			{
#if DEBUG_VM
				std::cout << "OP_PUSH_FUN" << std::endl;
#endif
				auto str = new std::string();
				r->str_lp_u64_dyn(*str);
				auto val = std::make_shared<Mixed>(str);
				val->type = Mixed::FUNC;
				push(std::move(val));
			}
			else if (op == OP_PUSH_VAR)
			{
#if DEBUG_VM
				std::cout << "OP_PUSH_VAR" << std::endl;
#endif
				auto str = new std::string();
				r->str_lp_u64_dyn(*str);
				auto val = std::make_shared<Mixed>(str);
				val->type = Mixed::VAR_NAME;
				push(std::move(val));
			}
			else if (op == OP_POP_ARGS)
			{
#if DEBUG_VM
				std::cout << "OP_POP_ARGS" << std::endl;
#endif
				uint64_t num_args;
				r->u64_dyn(num_args);
				std::vector<std::string> var_names{};
				var_names.reserve(num_args);
				while (num_args--)
				{
					var_names.emplace_back(popVarName());
				}
				for (auto& var_name : var_names)
				{
					if (stack.empty())
					{
						break;
					}
					vars.emplace(std::move(var_name), pop());
				}
			}
			else
			{
#if DEBUG_VM
				std::cout << "Op " << (int)op << std::endl;
#endif
				return true;
			}
		}
#if DEBUG_VM
		current_op = OP_PUSH_INT;
#endif
		return false;
	}

	void LangVm::push(Mixed&& val)
	{
		push(std::make_shared<Mixed>(val));
	}

	void LangVm::push(std::shared_ptr<Mixed> val)
	{
#if DEBUG_VM
		std::cout << "> Push " << val->getTypeName() << std::endl;
#endif
		stack.emplace(std::move(val));
	}

	std::shared_ptr<Mixed> LangVm::popRaw()
	{
		if (stack.empty())
		{
#if DEBUG_VM
			std::cout << "> Pop EMPTY" << std::endl;
#endif
			std::string err = "Op ";
			err.append(std::to_string(current_op));
			err.append(" failed to pop a value because the stack is empty");
			throw ParseError(std::move(err));
		}
		std::shared_ptr<Mixed> val = std::move(stack.top());
#if DEBUG_VM
		std::cout << "> Pop " << val->getTypeName() << std::endl;
#endif
		stack.pop();
		return val;
	}

	std::shared_ptr<Mixed> LangVm::pop()
	{
		auto val = popRaw();
		if (val->isVarName())
		{
			if (auto e = vars.find(val->getVarName()); e != vars.end())
			{
				return e->second;
			}
			std::string err = "Op ";
			err.append(std::to_string(current_op));
			err.append(" expected a value, found undefined variable ");
			err.append(val->getVarName());
			throw ParseError(std::move(err));
		}
		return val;
	}

	std::shared_ptr<Mixed>& LangVm::popVarRef()
	{
		auto name = popVarName();
		if (auto e = vars.find(name); e != vars.end())
		{
			return e->second;
		}
		return vars.emplace(std::move(name), std::make_shared<Mixed>()).first->second;
	}

	std::string LangVm::popVarName()
	{
		auto val = popRaw();
		if (!val->isVarName())
		{
			std::string err = "Op ";
			err.append(std::to_string(current_op));
			err.append(" expected a var name, found ");
			err.append(val->getTypeName());
			throw ParseError(std::move(err));
		}
		return val->getVarName();
	}

	std::string LangVm::popString()
	{
		auto val = pop();
		if (!val->isString())
		{
			std::string err = "Op ";
			err.append(std::to_string(current_op));
			err.append(" expected a string, found ");
			err.append(val->getTypeName());
			throw ParseError(std::move(err));
		}
		return std::move(val->getString());
	}

	StringReader LangVm::popFunc()
	{
		auto val = pop();
		if (!val->isFunc())
		{
			std::string err = "Op ";
			err.append(std::to_string(current_op));
			err.append(" expected a function, found ");
			err.append(val->getTypeName());
			throw ParseError(std::move(err));
		}
		return StringReader(std::move(val->getFunc()));
	}
}
