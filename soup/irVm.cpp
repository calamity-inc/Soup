#include "irVm.hpp"

#include "rand.hpp"

NAMESPACE_SOUP
{
	[[nodiscard]] static irVm::Variable oneret(std::vector<irVm::Variable>&& ret)
	{
		SOUP_ASSERT(ret.size() == 1);
		SOUP_MOVE_RETURN(ret[0]);
	}

	std::vector<irVm::Variable> irVm::execute(const irModule& m, const irFunction& fn)
	{
		for (const auto& local : fn.locals)
		{
			locals.emplace_back(Variable::fromArithmeticValue(soup::rand.t<uint64_t>(0, -1), local));
		}

		for (const auto& insn : fn.insns)
		{
			auto ret = execute(m, *insn);
			if (insn->type == IR_RET)
			{
				return ret;
			}
		}
		return {};
	}

	std::vector<irVm::Variable> irVm::execute(const irModule& m, const irExpression& insn)
	{
		switch (insn.type)
		{
		case IR_CONST_BOOL:
			SOUP_ASSERT(insn.children.size() == 0);
			return { Variable(insn.const_bool.value) };

		case IR_CONST_I8:
			SOUP_ASSERT(insn.children.size() == 0);
			return { Variable(insn.const_i8.value) };

		case IR_CONST_I32:
			SOUP_ASSERT(insn.children.size() == 0);
			return { Variable(insn.const_i32.value) };

		case IR_CONST_I64:
			SOUP_ASSERT(insn.children.size() == 0);
			return { Variable(insn.const_i64.value) };

		case IR_CONST_PTR:
			SOUP_ASSERT(insn.children.size() == 0);
			return { Variable(insn.const_ptr.value) };

		case IR_LOCAL_GET:
			SOUP_ASSERT(insn.children.size() == 0);
			return { Variable(locals.at(insn.local_get.index)) };

		case IR_LOCAL_SET:
			SOUP_ASSERT(insn.children.size() == 1);
			locals.at(insn.local_set.index) = oneret(execute(m, *insn.children[0]));
			return {};

		case IR_CALL:
			{
				irVm callvm(memory);
				for (const auto& child : insn.children)
				{
					callvm.locals.emplace_back(oneret(execute(m, *child)));
				}
				std::vector<Variable> ret;
				if (insn.call.index >= 0)
				{
					const auto& func = m.func_exports.at(insn.call.index);
					SOUP_ASSERT(func.parameters.size() == callvm.locals.size());
					ret = callvm.execute(m, func);
					SOUP_ASSERT(ret.size() == func.returns.size());
				}
				else
				{
					const auto& imp = m.imports.at(~insn.call.index);
					SOUP_ASSERT(imp.module_name == "posix" && imp.func.name == "write");
					SOUP_ASSERT(callvm.locals.size() == 3);
					SOUP_ASSERT(callvm.locals.at(0).type == IR_I32);
					SOUP_ASSERT(callvm.locals.at(0).value.i32 == 1);
					SOUP_ASSERT(callvm.locals.at(1).type == IR_PTR);
					SOUP_ASSERT(callvm.locals.at(2).type == IR_I64);
					fwrite(&memory[callvm.locals.at(1).value.ptr], 1, callvm.locals.at(2).value.i64, stdout);
					ret.emplace_back(Variable(callvm.locals.at(2).value.i64));
				}
				return ret;
			}

		case IR_RET:
			{
				std::vector<Variable> ret{};
				for (const auto& child : insn.children)
				{
					for (auto& var : execute(m, *child))
					{
						ret.emplace_back(std::move(var));
					}
				}
				return ret;
			}

		case IR_IFELSE:
			{
				auto cond = oneret(execute(m, *insn.children.at(0)));
				SOUP_ASSERT(cond.type == IR_BOOL);
				if (cond.value.b)
				{
					for (size_t i = 0; i != insn.ifelse.ifinsns; ++i)
					{
						execute(m, *insn.children[1 + i]);
					}
				}
				else
				{
					for (size_t i = 1 + insn.ifelse.ifinsns; i != insn.children.size(); ++i)
					{
						execute(m, *insn.children[i]);
					}
				}
			}
			return {};

		case IR_WHILE:
			while (true)
			{
				auto cond = oneret(execute(m, *insn.children.at(0)));
				SOUP_ASSERT(cond.type == IR_BOOL);
				if (!cond.value.b)
				{
					break;
				}
				for (size_t i = 1; i != insn.children.size(); ++i)
				{
					execute(m, *insn.children[i]);
				}
			}
			return {};

		case IR_DISCARD:
			{
				SOUP_ASSERT(insn.children.size() == 1);
				auto vars = execute(m, *insn.children[0]);
				for (size_t i = 0; i != insn.discard.count; ++i)
				{
					vars.pop_back();
				}
				return vars;
			}

		case IR_ADD_I32:
			{
				SOUP_ASSERT(insn.children.size() == 2);
				auto lhs = oneret(execute(m, *insn.children[0]));
				auto rhs = oneret(execute(m, *insn.children[1]));
				SOUP_ASSERT(lhs.type == rhs.type && lhs.type == IR_I32);
				return { Variable(lhs.value.i32 + rhs.value.i32) };
			}

		case IR_ADD_I64:
			{
				SOUP_ASSERT(insn.children.size() == 2);
				auto lhs = oneret(execute(m, *insn.children[0]));
				auto rhs = oneret(execute(m, *insn.children[1]));
				SOUP_ASSERT(lhs.type == rhs.type && lhs.type == IR_I64);
				return { Variable(lhs.value.i64 + rhs.value.i64) };
			}

		case IR_ADD_PTR:
			{
				SOUP_ASSERT(insn.children.size() == 2);
				auto lhs = oneret(execute(m, *insn.children[0]));
				auto rhs = oneret(execute(m, *insn.children[1]));
				SOUP_ASSERT(lhs.type == rhs.type && lhs.type == IR_PTR);
				return { Variable(lhs.value.ptr + rhs.value.ptr) };
			}

		case IR_SUB_I32:
			{
				SOUP_ASSERT(insn.children.size() == 2);
				auto lhs = oneret(execute(m, *insn.children[0]));
				auto rhs = oneret(execute(m, *insn.children[1]));
				SOUP_ASSERT(lhs.type == rhs.type && lhs.type == IR_I32);
				return { Variable(lhs.value.i32 - rhs.value.i32) };
			}

		case IR_SUB_I64:
			{
				SOUP_ASSERT(insn.children.size() == 2);
				auto lhs = oneret(execute(m, *insn.children[0]));
				auto rhs = oneret(execute(m, *insn.children[1]));
				SOUP_ASSERT(lhs.type == rhs.type && lhs.type == IR_I64);
				return { Variable(lhs.value.i64 - rhs.value.i64) };
			}

		case IR_MUL_I64:
			{
				SOUP_ASSERT(insn.children.size() == 2);
				auto lhs = oneret(execute(m, *insn.children[0]));
				auto rhs = oneret(execute(m, *insn.children[1]));
				SOUP_ASSERT(lhs.type == rhs.type && lhs.type == IR_I64);
				return { Variable(lhs.value.i64 * rhs.value.i64) };
			}

		case IR_SDIV_I64:
			{
				SOUP_ASSERT(insn.children.size() == 2);
				auto lhs = oneret(execute(m, *insn.children[0]));
				auto rhs = oneret(execute(m, *insn.children[1]));
				SOUP_ASSERT(lhs.type == rhs.type && lhs.type == IR_I64);
				return { Variable(lhs.value.i64 / rhs.value.i64) };
			}

		case IR_UDIV_I64:
			{
				SOUP_ASSERT(insn.children.size() == 2);
				auto lhs = oneret(execute(m, *insn.children[0]));
				auto rhs = oneret(execute(m, *insn.children[1]));
				SOUP_ASSERT(lhs.type == rhs.type && lhs.type == IR_I64);
				return { Variable(static_cast<uint64_t>(lhs.value.i64) / static_cast<uint64_t>(rhs.value.i64)) };
			}

		case IR_SMOD_I64:
			{
				SOUP_ASSERT(insn.children.size() == 2);
				auto lhs = oneret(execute(m, *insn.children[0]));
				auto rhs = oneret(execute(m, *insn.children[1]));
				SOUP_ASSERT(lhs.type == rhs.type && lhs.type == IR_I64);
				return { Variable(lhs.value.i64 % rhs.value.i64) };
			}

		case IR_UMOD_I64:
			{
				SOUP_ASSERT(insn.children.size() == 2);
				auto lhs = oneret(execute(m, *insn.children[0]));
				auto rhs = oneret(execute(m, *insn.children[1]));
				SOUP_ASSERT(lhs.type == rhs.type && lhs.type == IR_I64);
				return { Variable(static_cast<uint64_t>(lhs.value.i64) % static_cast<uint64_t>(rhs.value.i64)) };
			}

		case IR_EQUALS_I8:
			{
				SOUP_ASSERT(insn.children.size() == 2);
				auto lhs = oneret(execute(m, *insn.children[0]));
				auto rhs = oneret(execute(m, *insn.children[1]));
				SOUP_ASSERT(lhs.type == rhs.type && lhs.type == IR_I8);
				return { Variable(lhs.value.i8 == rhs.value.i8) };
			}

		case IR_EQUALS_I32:
			{
				SOUP_ASSERT(insn.children.size() == 2);
				auto lhs = oneret(execute(m, *insn.children[0]));
				auto rhs = oneret(execute(m, *insn.children[1]));
				SOUP_ASSERT(lhs.type == rhs.type && lhs.type == IR_I32);
				return { Variable(lhs.value.i32 == rhs.value.i32) };
			}

		case IR_EQUALS_I64:
			{
				SOUP_ASSERT(insn.children.size() == 2);
				auto lhs = oneret(execute(m, *insn.children[0]));
				auto rhs = oneret(execute(m, *insn.children[1]));
				SOUP_ASSERT(lhs.type == rhs.type && lhs.type == IR_I64);
				return { Variable(lhs.value.i64 == rhs.value.i64) };
			}

		case IR_NOTEQUALS_I8:
			{
				SOUP_ASSERT(insn.children.size() == 2);
				auto lhs = oneret(execute(m, *insn.children[0]));
				auto rhs = oneret(execute(m, *insn.children[1]));
				SOUP_ASSERT(lhs.type == rhs.type && lhs.type == IR_I8);
				return { Variable(lhs.value.i8 != rhs.value.i8) };
			}

		case IR_NOTEQUALS_I32:
			{
				SOUP_ASSERT(insn.children.size() == 2);
				auto lhs = oneret(execute(m, *insn.children[0]));
				auto rhs = oneret(execute(m, *insn.children[1]));
				SOUP_ASSERT(lhs.type == rhs.type && lhs.type == IR_I32);
				return { Variable(lhs.value.i32 != rhs.value.i32) };
			}

		case IR_NOTEQUALS_I64:
			{
				SOUP_ASSERT(insn.children.size() == 2);
				auto lhs = oneret(execute(m, *insn.children[0]));
				auto rhs = oneret(execute(m, *insn.children[1]));
				SOUP_ASSERT(lhs.type == rhs.type && lhs.type == IR_I64);
				return { Variable(lhs.value.i64 != rhs.value.i64) };
			}

		case IR_LOAD_I8:
			{
				auto ptr = oneret(execute(m, *insn.children.at(0)));
				SOUP_ASSERT(ptr.type == IR_PTR);
				return { Variable(static_cast<int8_t>(memory.at(ptr.value.ptr))) };
			}

		case IR_STORE_I8:
			{
				SOUP_ASSERT(insn.children.size() == 2);
				auto ptr = oneret(execute(m, *insn.children[0]));
				auto value = oneret(execute(m, *insn.children[1]));
				SOUP_ASSERT(ptr.type == IR_PTR);
				SOUP_ASSERT(value.type == IR_I8);
				*reinterpret_cast<int8_t*>(&memory.at(ptr.value.ptr)) = value.value.i8;
				return {};
			}

		case IR_STORE_I32:
			{
				SOUP_ASSERT(insn.children.size() == 2);
				auto ptr = oneret(execute(m, *insn.children[0]));
				auto value = oneret(execute(m, *insn.children[1]));
				SOUP_ASSERT(ptr.type == IR_PTR);
				SOUP_ASSERT(value.type == IR_I32);
				*reinterpret_cast<int32_t*>(&memory.at(ptr.value.ptr)) = value.value.i32;
			}
			return {};

		case IR_STORE_I64:
			{
				SOUP_ASSERT(insn.children.size() == 2);
				auto ptr = oneret(execute(m, *insn.children[0]));
				auto value = oneret(execute(m, *insn.children[1]));
				SOUP_ASSERT(ptr.type == IR_PTR);
				SOUP_ASSERT(value.type == IR_I64);
				*reinterpret_cast<int64_t*>(&memory.at(ptr.value.ptr)) = value.value.i64;
			}
			return {};

		case IR_I64_TO_PTR:
			{
				auto var = oneret(execute(m, *insn.children.at(0)));
				SOUP_ASSERT(var.type == IR_I64);
				return { Variable(static_cast<uint64_t>(var.value.i64)) };
			}

		case IR_I64_TO_I32:
			{
				auto var = oneret(execute(m, *insn.children.at(0)));
				SOUP_ASSERT(var.type == IR_I64);
				return { Variable(static_cast<int32_t>(var.value.i64)) };
			}

		case IR_I64_TO_I8:
			{
				auto var = oneret(execute(m, *insn.children.at(0)));
				SOUP_ASSERT(var.type == IR_I64);
				return { Variable(static_cast<int8_t>(var.value.i64)) };
			}

		case IR_I32_TO_I64_SX:
			{
				auto var = oneret(execute(m, *insn.children.at(0)));
				SOUP_ASSERT(var.type == IR_I32);
				return { Variable(static_cast<int64_t>(var.value.i32)) };
			}

		case IR_I32_TO_I64_ZX:
			{
				auto var = oneret(execute(m, *insn.children.at(0)));
				SOUP_ASSERT(var.type == IR_I32);
				return { Variable(static_cast<int64_t>(static_cast<uint64_t>(static_cast<uint32_t>(var.value.i32)))) };
			}

		case IR_I8_TO_I64_SX:
			{
				auto var = oneret(execute(m, *insn.children.at(0)));
				SOUP_ASSERT(var.type == IR_I8);
				return { Variable(static_cast<int64_t>(var.value.i8)) };
			}

		case IR_I8_TO_I64_ZX:
			{
				auto var = oneret(execute(m, *insn.children.at(0)));
				SOUP_ASSERT(var.type == IR_I8);
				return { Variable(static_cast<int64_t>(static_cast<uint64_t>(static_cast<uint8_t>(var.value.i8)))) };
			}
		}
		SOUP_UNREACHABLE;
	}

	irVm::Variable irVm::Variable::fromArithmeticValue(int64_t value, irType type)
	{
		switch (type)
		{
		case IR_BOOL: return static_cast<bool>(value);
		case IR_I8: return static_cast<int8_t>(value);
		case IR_I32: return static_cast<int32_t>(value);
		case IR_I64: return static_cast<int64_t>(value);
		case IR_PTR: return static_cast<uint64_t>(value);
		}
		SOUP_UNREACHABLE;
	}

	std::string irVm::Variable::toString() const noexcept
	{
		std::string str;
		switch (type)
		{
		case IR_BOOL:
			str = "IR_BOOL: ";
			str.append(value.b ? "true" : "false");
			break;

		case IR_I8:
			str = "IR_I8: ";
			str.append(std::to_string(value.i8));
			break;

		case IR_I32:
			str = "IR_I32: ";
			str.append(std::to_string(value.i32));
			break;

		case IR_I64:
			str = "IR_I64: ";
			str.append(std::to_string(value.i64));
			break;

		case IR_PTR:
			str = "IR_PTR: ";
			str.append(std::to_string(value.ptr));
			break;
		}
		return str;
	}
}
