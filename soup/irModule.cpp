#include "irModule.hpp"

NAMESPACE_SOUP
{
	std::string irModule::getContiguousMemory() const
	{
		std::string memory{};
		for (const auto& chunk : this->data)
		{
			size_t ptr = memory.size();
			if (size_t rem = ptr % chunk.alignment)
			{
				memory.append(chunk.alignment - rem, '\0');
			}
			memory.append(chunk.data);
		}
		return memory;
	}

	uint64_t irModule::allocateConstData(std::string&& data)
	{
		size_t ptr = 0;
		for (const auto& chunk : this->data)
		{
			if (size_t rem = ptr % chunk.alignment)
			{
				ptr += (chunk.alignment - rem);
			}
			if (chunk.is_const && chunk.data == data)
			{
				return ptr;
			}
			ptr += chunk.data.size();
		}
		auto& chunk = this->data.emplace_back();
		chunk.data = std::move(data);
		chunk.alignment = 1;
		chunk.is_const = true;
		return ptr;
	}

	uint64_t irModule::allocateZeroedMemory(uint64_t size, uint8_t align)
	{
		size_t ptr = 0;
		for (const auto& chunk : this->data)
		{
			if (size_t rem = ptr % chunk.alignment)
			{
				ptr += (chunk.alignment - rem);
			}
			ptr += chunk.data.size();
		}
		if (size_t rem = ptr % align)
		{
			ptr += (align - rem);
		}
		auto& chunk = this->data.emplace_back();
		chunk.data = std::string(size, '\0');
		chunk.alignment = align;
		chunk.is_const = false;
		return ptr;
	}

	intptr_t irModule::getFunctionIndex(const std::string& name)
	{
		intptr_t i = 0;
		for (; i != func_exports.size(); ++i)
		{
			if (func_exports[i].name == name)
			{
				return i;
			}
		}
		return i;
	}

	intptr_t irModule::getStrlenFunctionIndex()
	{
		intptr_t idx = getFunctionIndex("strlen");
		if (idx == func_exports.size())
		{
			irFunction& fn = func_exports.emplace_back();
			fn.name = "strlen";
			fn.parameters = { IR_PTR };
			fn.returns = { IR_I64 };
			fn.locals = { IR_I64 };
			{
				auto insn = soup::make_unique<irExpression>(IR_LOCAL_SET);
				insn->local_set.index = 1;
				{
					auto value = soup::make_unique<irExpression>(IR_CONST_I64);
					value->const_i64.value = 0;
					insn->children.emplace_back(std::move(value));
				}
				fn.insns.emplace_back(std::move(insn));
			}
			{
				auto loop = soup::make_unique<irExpression>(IR_WHILE);
				{
					auto condition = soup::make_unique<irExpression>(IR_NOTEQUALS_I8);
					{
						auto readExpr = soup::make_unique<irExpression>(IR_LOAD_I8);
						{
							auto addExpr = soup::make_unique<irExpression>(IR_ADD_PTR);
							{
								auto localGetExpr = soup::make_unique<irExpression>(IR_LOCAL_GET);
								localGetExpr->local_get.index = 0;
								addExpr->children.emplace_back(std::move(localGetExpr));
							}
							{
								auto i64ToPtrExpr = soup::make_unique<irExpression>(IR_I64_TO_PTR);
								{
									auto localGetExpr = soup::make_unique<irExpression>(IR_LOCAL_GET);
									localGetExpr->local_get.index = 1;
									i64ToPtrExpr->children.emplace_back(std::move(localGetExpr));
								}
								addExpr->children.emplace_back(std::move(i64ToPtrExpr));
							}
							readExpr->children.emplace_back(std::move(addExpr));
						}
						condition->children.emplace_back(std::move(readExpr));
					}
					{
						auto constExpr = soup::make_unique<irExpression>(IR_CONST_I8);
						constExpr->const_i8.value = 0;
						condition->children.emplace_back(std::move(constExpr));
					}
					loop->children.emplace_back(std::move(condition));
				}
				{
					auto setInsn = soup::make_unique<irExpression>(IR_LOCAL_SET);
					setInsn->local_set.index = 1;
					{
						auto addExpr = soup::make_unique<irExpression>(IR_ADD_I64);
						{
							auto getLocal = soup::make_unique<irExpression>(IR_LOCAL_GET);
							getLocal->local_get.index = 1;
							addExpr->children.emplace_back(std::move(getLocal));

							auto constInc = soup::make_unique<irExpression>(IR_CONST_I64);
							constInc->const_i64.value = 1;
							addExpr->children.emplace_back(std::move(constInc));
						}
						setInsn->children.emplace_back(std::move(addExpr));
					}
					loop->children.emplace_back(std::move(setInsn));
				}
				fn.insns.emplace_back(std::move(loop));
			}
			{
				auto returnInsn = soup::make_unique<irExpression>(IR_RET);
				{
					auto getLocal = soup::make_unique<irExpression>(IR_LOCAL_GET);
					getLocal->local_get.index = 1;
					returnInsn->children.emplace_back(std::move(getLocal));
				}
				fn.insns.emplace_back(std::move(returnInsn));
			}
		}
		return idx;
	}

	intptr_t irModule::getPrintFunctionIndex()
	{
		intptr_t idx = getFunctionIndex("print");
		if (idx == func_exports.size())
		{
			auto strlen_idx = getStrlenFunctionIndex();
			idx = func_exports.size();

			irFunction& fn = func_exports.emplace_back();
			fn.name = "print";
			fn.parameters.emplace_back(IR_PTR);
			{
				auto callWrite = soup::make_unique<irExpression>(IR_CALL);
				callWrite->call.index = getPosixWriteFunctionIndex();
				{
					auto hStdout = soup::make_unique<irExpression>(IR_CONST_I32);
					hStdout->const_i32.value = 1;
					callWrite->children.emplace_back(std::move(hStdout));
				}
				{
					auto getLocal = soup::make_unique<irExpression>(IR_LOCAL_GET);
					getLocal->local_get.index = 0;
					callWrite->children.emplace_back(std::move(getLocal));
				}
				{
					auto callStrlen = soup::make_unique<irExpression>(IR_CALL);
					callStrlen->call.index = strlen_idx;
					{
						auto getLocal = soup::make_unique<irExpression>(IR_LOCAL_GET);
						getLocal->local_get.index = 0;
						callStrlen->children.emplace_back(std::move(getLocal));
					}
					callWrite->children.emplace_back(std::move(callStrlen));
				}
				fn.insns.emplace_back(std::move(callWrite));
			}
		}
		return idx;
	}

	intptr_t irModule::getImportFunctionIndex(const std::string& mod, const std::string& func)
	{
		for (size_t i = 0; i != imports.size(); ++i)
		{
			if (imports[i].func.name == func
				&& imports[i].module_name == mod
				)
			{
				return ~i;
			}
		}
		return 0;
	}

	intptr_t irModule::getPosixWriteFunctionIndex()
	{
		if (auto idx = getImportFunctionIndex("posix", "write"))
		{
			return idx;
		}
		auto idx = imports.size();
		{
			auto& imp = imports.emplace_back();
			imp.module_name = "posix";
			imp.func.name = "write";
			imp.func.parameters = { IR_I32, IR_PTR, IR_I64 };
			imp.func.returns = { IR_I64 };
		}
		return ~idx;
	}

	irFunction& irModule::getFunction(intptr_t index)
	{
		if (index >= 0)
		{
			return func_exports.at(index);
		}
		else
		{
			return imports.at(~index).func;
		}
	}

	static void updateCallsImpl(intptr_t old_idx, intptr_t new_idx, irExpression& insn)
	{
		if (insn.type == IR_CALL)
		{
			if (insn.call.index == old_idx)
			{
				insn.call.index = new_idx;
			}
		}
		for (auto& child : insn.children)
		{
			updateCallsImpl(old_idx, new_idx, *child);
		}
	}

	void irModule::updateCalls(intptr_t old_idx, intptr_t new_idx)
	{
		for (auto& func : func_exports)
		{
			for (auto& insn : func.insns)
			{
				updateCallsImpl(old_idx, new_idx, *insn);
			}
		}
	}
}
