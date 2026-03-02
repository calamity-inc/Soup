#include "cli.hpp"

#include <iostream>

#include <FileReader.hpp>
#include <json.hpp>
#include <string.hpp>
#include <wasm.hpp>

#define WAIT_FOR_DEBUGGER false
#if WAIT_FOR_DEBUGGER
#include <os.hpp>
#endif

using namespace soup;

static std::vector<UniquePtr<int64_t>> externrefs;

[[nodiscard]] static bool instantiate_value(const JsonObject& desc, WasmValue& out)
{
	//std::cout << "instantiate_value: " << desc.encode() << "\n";
	out.type = wasm_type_from_string(desc.at("type").asStr());
#if SOUP_WASM_SIMD
	if (out.type == WASM_V128)
	{
		const auto& value = desc.at("value").asArr();
		if (value.size() == 2)
		{
			for (int i = 0; i != 2; ++i)
			{
				SOUP_RETHROW_FALSE(string::toIntOpt<uint64_t>(value.at(i).asStr(), string::TI_FULL).consume(reinterpret_cast<uint64_t&>(out.i64x2[i])));
			}
		}
		else if (value.size() == 4)
		{
			for (int i = 0; i != 4; ++i)
			{
				SOUP_RETHROW_FALSE(string::toIntOpt<uint32_t>(value.at(i).asStr(), string::TI_FULL).consume(reinterpret_cast<uint32_t&>(out.i32x4[i])));
			}
		}
		else if (value.size() == 8)
		{
			for (int i = 0; i != 8; ++i)
			{
				SOUP_RETHROW_FALSE(string::toIntOpt<uint16_t>(value.at(i).asStr(), string::TI_FULL).consume(reinterpret_cast<uint16_t&>(out.i16x8[i])));
			}
		}
		else
		{
			for (int i = 0; i != 16; ++i)
			{
				SOUP_RETHROW_FALSE(string::toIntOpt<uint8_t>(value.at(i).asStr(), string::TI_FULL).consume(reinterpret_cast<uint8_t&>(out.i8x16[i])));
			}
		}
		return true;
	}
#endif
	const std::string& value = desc.at("value").asStr();
	if (value == "null")
	{
		out.i64 = 0;
	}
	else
	{
		SOUP_RETHROW_FALSE(string::toIntOpt<uint64_t>(value, string::TI_FULL).consume(reinterpret_cast<uint64_t&>(out.i64)));
		if (out.type == WASM_FUNCREF)
		{
			out.i64 |= 0x1'0000'0000;
		}
		else if (out.type == WASM_EXTERNREF)
		{
			const int64_t* ptr = nullptr;
			for (const auto& ref : externrefs)
			{
				if (*ref == out.i64)
				{
					ptr = ref.get();
					break;
				}
			}
			if (ptr == nullptr)
			{
				ptr = externrefs.emplace_back(soup::make_unique<int64_t>(out.i64)).get();
			}
			out.i64 = reinterpret_cast<uintptr_t>(ptr);
		}
	}
	return true;
}

[[nodiscard]] static std::string stringify_value(const WasmValue& value)
{
	std::string str = value.toString();
	if (value.type == WASM_EXTERNREF && value.i64)
	{
		str.append(" (*-> ");
		str.append(std::to_string(*(uint64_t*)value.i64));
		str.push_back(')');
	}
	return str;
}

[[nodiscard]] static bool check_form(const WasmValue& actual, const JsonObject& expected)
{
#if SOUP_WASM_SIMD
	if (expected.at("value").isStr())
#endif
	{
		const std::string& type = expected.at("type").asStr();
		const std::string& value = expected.at("value").asStr();
		if (value == "nan:arithmetic")
		{
			SOUP_IF_UNLIKELY (type == "f32"
				? !std::isnan(actual.f32)
				: !std::isnan(actual.f64)
				)
			{
				return false;
			}
		}
		else if (value == "nan:canonical")
		{
			SOUP_IF_UNLIKELY (type == "f32"
				? (actual.i32 != 0x7fc00000 && actual.i32 != 0xffc00000)
				: (actual.i64 != 0x7ff8000000000000 && actual.i64 != 0xfff8000000000000)
				)
			{
				return false;
			}
		}
	}
#if SOUP_WASM_SIMD
	else
	{
		const auto& value = expected.at("value").asArr();
		if (value.size() == 2)
		{
			for (int i = 0; i != 2; ++i)
			{
				if (value.at(i).asStr() == "nan:arithmetic")
				{
					SOUP_RETHROW_FALSE(std::isnan(actual.f64x2[i]));
				}
				else if (value.at(i).asStr() == "nan:canonical")
				{
					SOUP_RETHROW_FALSE(actual.i64x2[i] == 0x7ff8000000000000 || actual.i64x2[i] == 0xfff8000000000000);
				}
				else
				{
					uint64_t u;
					SOUP_RETHROW_FALSE(string::toIntOpt<uint64_t>(value.at(i).asStr(), string::TI_FULL).consume(u));
					SOUP_RETHROW_FALSE(actual.i64x2[i] == u);
				}
			}
		}
		else if (value.size() == 4)
		{
			for (int i = 0; i != 4; ++i)
			{
				if (value.at(i).asStr() == "nan:arithmetic")
				{
					SOUP_RETHROW_FALSE(std::isnan(actual.f32x4[i]));
				}
				else if (value.at(i).asStr() == "nan:canonical")
				{
					SOUP_RETHROW_FALSE(actual.i32x4[i] == 0x7fc00000 || actual.i32x4[i] == 0xffc00000);
				}
				else
				{
					uint32_t u;
					SOUP_RETHROW_FALSE(string::toIntOpt<uint32_t>(value.at(i).asStr(), string::TI_FULL).consume(u));
					SOUP_RETHROW_FALSE(actual.i32x4[i] == u);
				}
			}
		}
		else
		{
			return false;
		}
	}
#endif
	return true;
}

#define API_CHECK_STACK(x) SOUP_IF_UNLIKELY (vm.stack.size() < x) { SOUP_THROW(Exception("Insufficient values on stack for function call")); }

int cli_wast(const std::string& file)
{
#if WAIT_FOR_DEBUGGER
	std::cout << "Attach debugger now." << std::endl;
	os::sleep(5000);
	std::cout << "Starting." << std::endl;
#endif
	if (auto jr = json::decode(string::fromFile(file)))
	{
		// https://github.com/WebAssembly/spec/blob/main/interpreter/host/spectest.ml
		const std::unordered_map<std::string, wasm_ffi_func_t> spectest_functions{
			{
				"print_i32",
				[](WasmVm& vm, uint32_t func_index, const WasmFunctionType&)
				{
					API_CHECK_STACK(1);
					vm.stack.pop_back();
				}
			},
			{
				"print_i64",
				[](WasmVm& vm, uint32_t func_index, const WasmFunctionType&)
				{
					API_CHECK_STACK(1);
					vm.stack.pop_back();
				}
			},
			{
				"print_f32",
				[](WasmVm& vm, uint32_t func_index, const WasmFunctionType&)
				{
					API_CHECK_STACK(1);
					vm.stack.pop_back();
				}
			},
			{
				"print_f64",
				[](WasmVm& vm, uint32_t func_index, const WasmFunctionType&)
				{
					API_CHECK_STACK(1);
					vm.stack.pop_back();
				}
			},
			{
				"print_i32_f32",
				[](WasmVm& vm, uint32_t func_index, const WasmFunctionType&)
				{
					API_CHECK_STACK(2);
					vm.stack.pop_back();
					vm.stack.pop_back();
				}
			},
			{
				"print_f64_f64",
				[](WasmVm& vm, uint32_t func_index, const WasmFunctionType&)
				{
					API_CHECK_STACK(2);
					vm.stack.pop_back();
					vm.stack.pop_back();
				}
			},
			{
				"print",
				[](WasmVm& vm, uint32_t func_index, const WasmFunctionType&)
				{
					// This function is apparently overloaded, so in theory it might have to pop a variable number of arguments.
				}
			},
		};
		const std::unordered_map<std::string, SharedPtr<WasmValue>> spectest_globals{
			{ "global_i32", soup::make_shared<WasmValue>((uint32_t)666) },
			{ "global_i64", soup::make_shared<WasmValue>((uint64_t)666) },
			{ "global_f32", soup::make_shared<WasmValue>(666.6f) },
			{ "global_f64", soup::make_shared<WasmValue>(666.6) },
		};
		const std::unordered_map<std::string, SharedPtr<WasmScript::Table>> spectest_tables{
			{ "table", soup::make_shared<WasmScript::Table>(WASM_FUNCREF, 10, 20, false) },
			{ "table64", soup::make_shared<WasmScript::Table>(WASM_FUNCREF, 10, 20, true) },
		};
		const auto spectest_memory = soup::make_shared<WasmScript::Memory>(1, 2, false);

		WasmSharedEnvironment shared_env;
		WasmScript* scr = &shared_env.createScript();
		std::unordered_map<std::string, WasmScript*> named_modules;
		std::vector<std::pair<std::string, WasmScript*>> registered_module;
		SOUP_TRY
		{
			for (const auto& cmd_entry : jr->asObj().at("commands").asArr())
			{
				const auto& cmd = cmd_entry.asObj();
				const auto& type = cmd.at("type").asStr();
				if (type == "module")
				{
					FileReader fr(cmd.at("filename").asStr());
					scr = &shared_env.createScript();
					//std::cout << "Loading " << cmd.at("filename").asStr().value << " (defined on line " << cmd.at("line").asInt().value << ")" << std::endl;
					SOUP_IF_UNLIKELY (!scr->load(fr))
					{
						std::cout << "Failed to load module " << cmd.at("filename").reinterpretAsStr().value << " (defined on line " << cmd.at("line").asInt().value << ")" << std::endl;
						return 1;
					}
					scr->provideImportedFunctions("spectest", spectest_functions);
					scr->provideImportedGlobals("spectest", spectest_globals);
					scr->provideImportedTables("spectest", spectest_tables);
					scr->provideImportedMemory("spectest", "memory", spectest_memory);
					for (const auto& mod : registered_module)
					{
						scr->importFromModule(mod.first, *mod.second);
					}
					SOUP_IF_UNLIKELY (scr->hasUnresolvedImports())
					{
						std::cout << "Warning: Unresolved imports for module " << cmd.at("filename").reinterpretAsStr().value << " (defined on line " << cmd.at("line").asInt().value << ")" << std::endl;
					}
					SOUP_IF_UNLIKELY (!scr->instantiate())
					{
						std::cout << "Failed to instantiate module " << cmd.at("filename").reinterpretAsStr().value << " (defined on line " << cmd.at("line").asInt().value << ")" << std::endl;
						return 1;
					}
					if (cmd.contains("name"))
					{
						named_modules.emplace(cmd.at("name").asStr().value, scr);
					}
				}
				else if (type == "register")
				{
					registered_module.emplace_back(cmd.at("as").asStr().value, scr);
				}
				else if (type == "assert_malformed")
				{
					FileReader fr(cmd.at("filename").asStr());
					WasmSharedEnvironment::ScriptRaii tmp(shared_env.createScript());
					SOUP_IF_UNLIKELY (tmp->load(fr))
					{
						std::cout << "Did not fail to load malformed module " << cmd.at("filename").reinterpretAsStr().value << " (defined on line " << cmd.at("line").asInt().value << ")" << std::endl;
						goto _wast_on_error;
					}
				}
				else if (type == "assert_unlinkable")
				{
					FileReader fr(cmd.at("filename").asStr());
					WasmSharedEnvironment::ScriptRaii tmp(shared_env.createScript());
					SOUP_IF_UNLIKELY(!tmp->load(fr))
					{
						std::cout << "Failed to load module " << cmd.at("filename").reinterpretAsStr().value << " (defined on line " << cmd.at("line").asInt().value << ")" << std::endl;
						return 1;
					}
					tmp->provideImportedFunctions("spectest", spectest_functions);
					tmp->provideImportedGlobals("spectest", spectest_globals);
					tmp->provideImportedTables("spectest", spectest_tables);
					tmp->provideImportedMemory("spectest", "memory", spectest_memory);
					for (const auto& mod : registered_module)
					{
						tmp->importFromModule(mod.first, *mod.second);
					}
					SOUP_IF_UNLIKELY (!tmp->hasUnresolvedImports())
					{
						std::cout << "Did not fail to link module " << cmd.at("filename").reinterpretAsStr().value << " (defined on line " << cmd.at("line").asInt().value << ")" << std::endl;
						goto _wast_on_error;
					}
				}
				else if (type == "assert_uninstantiable")
				{
					FileReader fr(cmd.at("filename").asStr());
					WasmSharedEnvironment::ScriptRaii tmp(shared_env.createScript());
					SOUP_IF_UNLIKELY (!tmp->load(fr))
					{
						std::cout << "Failed to load module " << cmd.at("filename").reinterpretAsStr().value << " (defined on line " << cmd.at("line").asInt().value << ")" << std::endl;
						return 1;
					}
					tmp->provideImportedFunctions("spectest", spectest_functions);
					tmp->provideImportedGlobals("spectest", spectest_globals);
					tmp->provideImportedTables("spectest", spectest_tables);
					tmp->provideImportedMemory("spectest", "memory", spectest_memory);
					for (const auto& mod : registered_module)
					{
						tmp->importFromModule(mod.first, *mod.second);
					}
					SOUP_IF_UNLIKELY (tmp->hasUnresolvedImports())
					{
						std::cout << "Warning: Unresolved imports for module " << cmd.at("filename").reinterpretAsStr().value << " (defined on line " << cmd.at("line").asInt().value << ")" << std::endl;
					}
					SOUP_IF_UNLIKELY (tmp->instantiate())
					{
						std::cout << "Did not fail to instantiate malformed module " << cmd.at("filename").reinterpretAsStr().value << " (defined on line " << cmd.at("line").asInt().value << ")" << std::endl;
						goto _wast_on_error;
					}
				}
				else if (type == "assert_invalid")
				{
					// Soup doesn't do static validation
				}
				else
				{
					std::vector<WasmValue> stack;
					if (cmd.contains("action"))
					{
						const auto& action = cmd.at("action").asObj();
						WasmScript* action_scr = scr;
						if (action.contains("module"))
						{
							action_scr = named_modules.at(action.at("module").asStr().value);
						}
						if (action.at("type").asStr() == "invoke")
						{
							const auto func_idx = action_scr->getExportedFuntion2(action.at("field").asStr());
							SOUP_IF_UNLIKELY (func_idx == -1)
							{
								std::cout << "Could not find export " << action.at("field").reinterpretAsStr().value << " for test at line " << cmd.at("line").asInt().value << std::endl;
								goto _wast_on_error;
							}
							//std::cout << "running code from line " << cmd.at("line").asInt().value << std::endl;
							std::vector<WasmValue> args;
							for (const auto& arg : action.at("args").asArr())
							{
								SOUP_IF_UNLIKELY (!instantiate_value(arg.asObj(), args.emplace_back()))
								{
									std::cout << "Failed to instantiate value: " << arg.encode() << std::endl;
									goto _wast_on_error;
								}
							}
							if (!action_scr->call(func_idx, std::move(args), &stack))
							{
								SOUP_IF_UNLIKELY (type != "assert_trap" && type != "assert_exhaustion" && type != "assert_exception")
								{
									std::cout << "Execution failed for test at line " << cmd.at("line").asInt().value << std::endl;
									goto _wast_on_error;
								}
							}
							else
							{
								SOUP_IF_UNLIKELY (type == "assert_trap" || type == "assert_exhaustion" || type == "assert_exception")
								{
									std::cout << "Execution did not fail for test at line " << cmd.at("line").asInt().value << std::endl;
									goto _wast_on_error;
								}
							}
						}
						else if (action.at("type").asStr() == "get")
						{
							const auto global = action_scr->getExportedGlobal(action.at("field").asStr());
							SOUP_IF_UNLIKELY (!global)
							{
								std::cout << "Could not find export " << action.at("field").reinterpretAsStr().value << " for test at line " << cmd.at("line").asInt().value << std::endl;
								goto _wast_on_error;
							}
							stack.emplace_back(*global);
						}
						else
						{
							std::cout << "Unknown action type: " << action.at("type").asStr().value << std::endl;
						}
					}
					if (type == "assert_return")
					{
						const auto& expected_arr = cmd.at("expected").asArr();
						for (auto i = expected_arr.children.rbegin(); i != expected_arr.children.rend(); ++i)
						{
							SOUP_IF_UNLIKELY (stack.empty())
							{
								std::cout << "Stack too empty for test at line " << cmd.at("line").asInt().value << std::endl;
								goto _wast_on_error;
							}
							const auto& expected = (*i)->asObj();
							if (WasmValue expected_vw; instantiate_value(expected, expected_vw))
							{
								SOUP_IF_UNLIKELY (stack.back() != expected_vw)
								{
									std::cout << "Return value mismatch for test at line " << cmd.at("line").asInt().value << std::endl;
									std::cout << "- Expected: " << stringify_value(expected_vw) << std::endl;
									std::cout << "- Actual: " << stringify_value(stack.back()) << std::endl;
									goto _wast_on_error;
								}
							}
							else
							{
								SOUP_IF_UNLIKELY (!check_form(stack.back(), expected))
								{
									std::cout << "Return value mismatch for test at line " << cmd.at("line").asInt().value << std::endl;
									std::cout << "- Expected: " << expected.encode() << std::endl;
									std::cout << "- Actual: " << stringify_value(stack.back()) << std::endl;
									goto _wast_on_error;
								}
							}
							stack.pop_back();
						}
						// When code uses 'return', there may be superfluous values on the stack. This doesn't affect VM semantics, tho.
						/*SOUP_IF_UNLIKELY (!stack.empty())
						{
							std::cout << "Stack too full for test at line " << cmd.at("line").asInt().value << std::endl;
							goto _wast_on_error;
						}*/
					}
					else if (type != "action" && type != "assert_trap" && type != "assert_exhaustion" && type != "assert_exception")
					{
						std::cout << "Unknown command type: " << type.value << std::endl;
					}
				}
				continue;
			_wast_on_error:
				return 1;
			}
		}
		SOUP_CATCH (std::exception, e)
		{
			std::cout << e.what() << std::endl;
			return 1;
		}
	}
	else
	{
		std::cout << "Input file is not valid JSON (use wast2json if need be)" << std::endl;
		return 1;
	}
	std::cout << "OK" << std::endl;
	return 0;
}
