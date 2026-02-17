#include "cli.hpp"

#include <iostream>

#include <FileReader.hpp>
#include <json.hpp>
#include <string.hpp>
#include <wasm.hpp>

using namespace soup;

static std::vector<UniquePtr<int64_t>> externrefs;

static void instantiate_value(const JsonObject& desc, WasmValue& out)
{
	const std::string& value = desc.at("value").asStr();
	out.type = wasm_type_from_string(desc.at("type").asStr());
	if (value == "null")
	{
		out.i64 = 0;
	}
	else
	{
		out.i64 = string::toIntOpt<uint64_t>(value, string::TI_FULL).value();
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
}

int cli_wast(const std::string& file)
{
	/*std::cout << "Attach debugger now." << std::endl;
	Sleep(5000);
	std::cout << "Starting." << std::endl;*/
	if (auto jr = json::decode(string::fromFile(file)))
	{
		SharedPtr<WasmScript> scr = soup::make_shared<WasmScript>();
		std::unordered_map<std::string, SharedPtr<WasmScript>> named_modules;
		std::vector<std::pair<std::string, SharedPtr<WasmScript>>> registered_module;
		try
		{
			for (const auto& cmd_entry : jr->asObj().at("commands").asArr())
			{
				const auto& cmd = cmd_entry.asObj();
				const auto& type = cmd.at("type").asStr();
				if (type == "module")
				{
					FileReader fr(cmd.at("filename").asStr());
					scr = soup::make_shared<WasmScript>();
					SOUP_IF_UNLIKELY (!scr->load(fr))
					{
						std::cout << "Failed to load module " << cmd.at("filename").reinterpretAsStr().value << " (defined on line " << cmd.at("line").asInt().value << ")" << std::endl;
						return 1;
					}
					scr->linkSpectestShim();
					for (const auto& mod : registered_module)
					{
						scr->importFromModule(mod.first, mod.second);
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
					WasmScript tmp;
					SOUP_IF_UNLIKELY (tmp.load(fr))
					{
						std::cout << "Did not fail to load malformed module " << cmd.at("filename").reinterpretAsStr().value << " (defined on line " << cmd.at("line").asInt().value << ")" << std::endl;
						goto _wast_next_cmd;
					}
				}
				else if (type == "assert_uninstantiable")
				{
					FileReader fr(cmd.at("filename").asStr());
					WasmScript tmp;
					SOUP_IF_UNLIKELY (!tmp.load(fr))
					{
						std::cout << "Failed to load module " << cmd.at("filename").reinterpretAsStr().value << " (defined on line " << cmd.at("line").asInt().value << ")" << std::endl;
						return 1;
					}
					tmp.linkSpectestShim();
					SOUP_IF_UNLIKELY (tmp.instantiate())
					{
						std::cout << "Did not fail to instantiate malformed module " << cmd.at("filename").reinterpretAsStr().value << " (defined on line " << cmd.at("line").asInt().value << ")" << std::endl;
						goto _wast_next_cmd;
					}
				}
				else if (type == "assert_invalid")
				{
					// Soup doesn't do static validation
				}
				else
				{
					std::stack<WasmValue> stack;
					if (cmd.contains("action"))
					{
						const auto& action = cmd.at("action").asObj();
						if (action.contains("module"))
						{
							scr = named_modules.at(action.at("module").asStr().value);
						}
						const auto func_idx = scr->getExportedFuntion2(action.at("field").asStr());
						SOUP_IF_UNLIKELY (func_idx == -1)
						{
							std::cout << "Could not find export " << action.at("field").reinterpretAsStr().value << " for test at line " << cmd.at("line").asInt().value << std::endl;
							goto _wast_next_cmd;
						}
						//std::cout << "running code from line " << cmd.at("line").asInt().value << std::endl;
						std::vector<WasmValue> args;
						for (const auto& arg : action.at("args").asArr())
						{
							instantiate_value(arg.asObj(), args.emplace_back());
						}
						if (!scr->call(func_idx, std::move(args), &stack))
						{
							SOUP_IF_UNLIKELY (type != "assert_trap" && type != "assert_exhaustion")
							{
								std::cout << "Execution failed for test at line " << cmd.at("line").asInt().value << std::endl;
								goto _wast_next_cmd;
							}
						}
						else
						{
							SOUP_IF_UNLIKELY (type == "assert_trap" || type == "assert_exhaustion")
							{
								std::cout << "Execution did not fail for test at line " << cmd.at("line").asInt().value << std::endl;
								goto _wast_next_cmd;
							}
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
								goto _wast_next_cmd;
							}
							const auto& expected = (*i)->asObj();
							const std::string& type = expected.at("type").asStr();
							const std::string& value = expected.at("value").asStr();
							if (value == "nan:arithmetic" || value == "nan:canonical")
							{
								SOUP_IF_UNLIKELY (type == "f32"
									? !std::isnan(stack.top().f32)
									: !std::isnan(stack.top().f64)
								)
								{
									std::cout << "Return value was not NaN for test at line " << cmd.at("line").asInt().value << std::endl;
									goto _wast_next_cmd;
								}
							}
							/*else if (value == "nan:canonical")
							{
								if (type == "f32"
									? stack.top().i32 != 0x400000
									: stack.top().i64 != 0x8000000000000ll
									)
								{
									std::cout << "Return value was not nan:canonical for test at line " << cmd.at("line").asInt().value << std::endl;
									goto _wast_next_cmd;
								}
							}*/
							else
							{
								WasmValue expected_vw;
								instantiate_value(expected, expected_vw);
								SOUP_IF_UNLIKELY (stack.top() != expected_vw)
								{
									std::cout << "Return value mismatch for test at line " << cmd.at("line").asInt().value << std::endl;
									if (value == "null")
									{
										std::cout << "- Expected: <" << type << "> 0 (null)" << std::endl;
									}
									else
									{
										std::cout << "- Expected: <" << type << "> " << (uint64_t)expected_vw.i64;
										if (expected_vw.type == WASM_EXTERNREF)
										{
											std::cout << " (*-> " << *(uint64_t*)expected_vw.i64 << ")";
										}
										std::cout << std::endl;
									}
									std::cout << "- Actual: <" << wasm_type_to_string(stack.top().type) << "> " << (uint64_t)stack.top().i64;
									if (stack.top().type == WASM_EXTERNREF)
									{
										std::cout << " (*-> " << *(uint64_t*)stack.top().i64 << ")";
									}
									std::cout << std::endl;
									goto _wast_next_cmd;
								}
							}
							stack.pop();
						}
						// When code uses 'return', there may be superfluous values on the stack. This doesn't affect VM semantics, tho.
						/*SOUP_IF_UNLIKELY (!stack.empty())
						{
							std::cout << "Stack too full for test at line " << cmd.at("line").asInt().value << std::endl;
							goto _wast_next_cmd;
						}*/
					}
					else if (type != "action" && type != "assert_trap" && type != "assert_exhaustion")
					{
						std::cout << "Unknown command type: " << type.value << std::endl;
					}
				}
			_wast_next_cmd:;
			}
		}
		catch (const std::exception& e)
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
	std::cout << "Done." << std::endl;
	return 0;
}
