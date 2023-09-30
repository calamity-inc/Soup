#include "LangVm.hpp"

#include <typeinfo> // bad_cast

#include "AllocRaiiVirtual.hpp"
#include "AssemblyBuilder.hpp"
#include "BuiltinOp.hpp"
#include "Exception.hpp"
#include "ParseError.hpp"
#include "StringReader.hpp"
#include "UniquePtr.hpp"

#define DEBUG_VM false

#if DEBUG_VM
#include <iostream>
#endif

namespace soup
{
	LangVm::LangVm(Reader& r, std::stack<SharedPtr<Mixed>>&& stack) noexcept
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
		while (r.u8(op))
		{
			current_op = op;
			if (op == OP_PUSH_STR)
			{
#if DEBUG_VM
				std::cout << "OP_PUSH_STR" << std::endl;
#endif
				std::string str;
				r.str_lp_u64_dyn(str);
				push(std::move(str));
			}
			else if (op == OP_PUSH_INT)
			{
#if DEBUG_VM
				std::cout << "OP_PUSH_INT" << std::endl;
#endif
				int64_t val;
				r.i64_dyn(val);
				push(soup::make_shared<Mixed>(val));
			}
			else if (op == OP_PUSH_UINT)
			{
#if DEBUG_VM
				std::cout << "OP_PUSH_UINT" << std::endl;
#endif
				uint64_t val;
				r.u64_dyn(val);
				push(soup::make_shared<Mixed>(val));
			}
			else if (op == OP_PUSH_FUN)
			{
#if DEBUG_VM
				std::cout << "OP_PUSH_FUN" << std::endl;
#endif
				auto str = new std::string();
				r.str_lp_u64_dyn(*str);
				auto val = soup::make_shared<Mixed>(str);
				val->type = Mixed::FUNC;
				push(std::move(val));
			}
			else if (op == OP_PUSH_VAR)
			{
#if DEBUG_VM
				std::cout << "OP_PUSH_VAR" << std::endl;
#endif
				auto str = new std::string();
				r.str_lp_u64_dyn(*str);
				auto val = soup::make_shared<Mixed>(str);
				val->type = Mixed::VAR_NAME;
				push(std::move(val));
			}
			else if (op == OP_POP_ARGS)
			{
#if DEBUG_VM
				std::cout << "OP_POP_ARGS" << std::endl;
#endif
				uint64_t num_args;
				r.u64_dyn(num_args);
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
						SOUP_THROW(ParseError("Function got less arguments than expected"));
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

	void LangVm::addOpcode(uint8_t opcode, LangVm::op_t op)
	{
		SOUP_IF_UNLIKELY (opcode != ops.size())
		{
			SOUP_THROW(Exception("Opcode registered out of order"));
		}
		ops.emplace_back(op);
	}

	void LangVm::push(Mixed&& val)
	{
		push(soup::make_shared<Mixed>(val));
	}

	void LangVm::execute()
	{
		for (uint8_t op; getNextOp(op); )
		{
			ops.at(op)(*this);
		}
	}

	static void asmvm_pushInt(LangVm& rcx, int64_t rdx)
	{
		rcx.push(soup::make_shared<Mixed>(rdx));
	}

	static void asmvm_pushUint(LangVm& rcx, uint64_t rdx)
	{
		rcx.push(soup::make_shared<Mixed>(rdx));
	}

	void LangVm::assembleAndExecute()
	{
		AssemblyBuilder b;
		b.funcBegin();
		b.set12toC(); // LangVm& is in RCX, moving to nonvolatile R12
		for (uint8_t op; r.u8(op); )
		{
			switch (op)
			{
			case OP_PUSH_STR:
			case OP_PUSH_FUN:
			case OP_PUSH_VAR:
			case OP_POP_ARGS:
				SOUP_THROW(ParseError("Can't assemble this (yet)"));

			case OP_PUSH_INT:
			{
				int64_t val;
				r.i64_dyn(val);
				b.setCto12();
				b.setD((uint64_t)val);
				b.setA((uint64_t)&asmvm_pushInt);
				b.callA();
				break;
			}

			case OP_PUSH_UINT:
			{
				uint64_t val;
				r.u64_dyn(val);
				b.setCto12();
				b.setD(val);
				b.setA((uint64_t)&asmvm_pushUint);
				b.callA();
				break;
			}

			default:
				b.setCto12();
				b.setA((uint64_t)ops.at(op));
				b.callA();
				break;
			}
		}
		b.funcEnd();
		auto f = b.allocate();
		using ft = void(*)(LangVm&);
		((ft)f->addr)(*this);
	}

	void LangVm::push(SharedPtr<Mixed> val)
	{
#if DEBUG_VM
		std::cout << "> Push " << val->getTypeName() << std::endl;
#endif
		stack.emplace(std::move(val));
	}

	SharedPtr<Mixed> LangVm::popRaw()
	{
		if (stack.empty())
		{
#if DEBUG_VM
			std::cout << "> Pop EMPTY" << std::endl;
#endif
			std::string err = "Op ";
			err.append(std::to_string(current_op));
			err.append(" failed to pop a value because the stack is empty");
			SOUP_THROW(ParseError(std::move(err)));
		}
		SharedPtr<Mixed> val = std::move(stack.top());
#if DEBUG_VM
		std::cout << "> Pop " << val->getTypeName() << std::endl;
#endif
		stack.pop();
		return val;
	}

	SharedPtr<Mixed> LangVm::pop()
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
			SOUP_THROW(ParseError(std::move(err)));
		}
		return val;
	}

	SharedPtr<Mixed>& LangVm::popVarRef()
	{
		auto name = popVarName();
		if (auto e = vars.find(name); e != vars.end())
		{
			return e->second;
		}
		return vars.emplace(std::move(name), soup::make_shared<Mixed>()).first->second;
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
			SOUP_THROW(ParseError(std::move(err)));
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
			SOUP_THROW(ParseError(std::move(err)));
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
			SOUP_THROW(ParseError(std::move(err)));
		}
		return StringReader(std::move(val->getFunc()));
	}
}
