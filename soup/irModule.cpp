#include "irModule.hpp"

namespace soup
{
	uint32_t irModule::getFunctionIndex(const std::string& name)
	{
		uint32_t i = 0;
		for (; i != func_exports.size(); ++i)
		{
			if (func_exports[i].name == name)
			{
				return i;
			}
		}
		return i;
	}

	uint32_t irModule::getStrlenFunctionIndex()
	{
		uint32_t idx = getFunctionIndex("strlen");
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

	uint32_t irModule::getPrintFunctionIndex()
	{
		uint32_t idx = getFunctionIndex("print");
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

	int64_t irModule::getImportFunctionIndex(const std::string& mod, const std::string& func)
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

	int64_t irModule::getPosixWriteFunctionIndex()
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

	static void updateCallsImpl(int64_t old_idx, int64_t new_idx, irExpression& insn)
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

	void irModule::updateCalls(int64_t old_idx, int64_t new_idx)
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
