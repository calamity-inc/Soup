#include "wasm.hpp"

#include <cstring> // memset

#include "Reader.hpp"
#include "StringRefReader.hpp"

#define DEBUG_LOAD false
#define DEBUG_VM false

#if DEBUG_LOAD || DEBUG_VM
#include <iostream>
#include "string.hpp"
#endif

// Good resources:
// - https://webassembly.github.io/wabt/demo/wat2wasm/
// - https://github.com/sunfishcode/wasm-reference-manual/blob/master/WebAssembly.md

namespace soup
{
	// WasmScript

	WasmScript::WasmScript()
	{
		memset(memory, 0, sizeof(memory));
	}

	bool WasmScript::load(const std::string& data)
	{
		StringRefReader r(data);
		return load(r);
	}

	bool WasmScript::load(Reader& r)
	{
		uint32_t u;
		r.u32_le(u);
		if (u != 1836278016) // magic - '\0asm' in little-endian
		{
			return false;
		}
		r.u32_le(u);
		if (u != 1) // version
		{
			return false;
		}
		while (r.hasMore())
		{
			uint8_t section_type;
			r.u8(section_type);
			size_t section_size;
			r.oml(section_size);
			switch (section_type)
			{
			default:
#if DEBUG_LOAD
				std::cout << "Ignoring section of type " << (int)section_type << "\n";
#endif
				SOUP_IF_UNLIKELY (section_size == 0)
				{
					return false;
				}
				r.skip(section_size);
				break;

#if false
			case 1: // Type
			{
				size_t num_types;
				r.oml(num_types);
#if DEBUG_LOAD
				std::cout << num_types << " type(s)\n";
#endif
				while (num_types--)
					{
						uint8_t type_type;
						r.u8(type_type);
						switch (type_type)
						{
						default:
							return false;

						case 0x60: // func
						{
#if DEBUG_LOAD
							std::cout << "- function with ";
#endif
							size_t n;
							r.oml(n); r.skip(n); // parameters
#if DEBUG_LOAD
							std::cout << n << " parameter(s) and ";
#endif
							r.oml(n); r.skip(n); // results
#if DEBUG_LOAD
							std::cout << n << " return value(s)\n";
#endif
						}
						break;
						}
					}
				}
				break;
#endif

			case 2: // Import
				{
					size_t num_imports;
					r.oml(num_imports);
#if DEBUG_LOAD
					std::cout << num_imports << " import(s)\n";
#endif
					while (num_imports--)
					{
						size_t module_name_len; r.oml(module_name_len);
						std::string module_name; r.str(module_name_len, module_name);
						size_t field_name_len; r.oml(field_name_len);
						std::string field_name; r.str(field_name_len, field_name);
#if DEBUG_LOAD
						std::cout << "-" << module_name << ":" << field_name << "\n";
#endif
						uint8_t kind; r.u8(kind);
						if (kind == 0) // function
						{
							size_t signature_index; r.oml(signature_index);
							function_imports.emplace_back(FunctionImport{ std::move(module_name), std::move(field_name), nullptr });
						}
					}
				}
				break;

			case 3: // Function
				{
					size_t num_functions;
					r.oml(num_functions);
#if DEBUG_LOAD
					std::cout << num_functions << " function(s)\n";
#endif
					while (num_functions--)
					{
						size_t signature_index; r.oml(signature_index);
					}
				}
				break;

			// 5 - Memory

			case 6: // Global
				{
					size_t num_globals;
					r.oml(num_globals);
					while (num_globals--)
					{
						uint8_t type; r.u8(type);
						SOUP_IF_UNLIKELY (type != 0x7f) // i32
						{
							return false;
						}
						r.skip(1); // mutability
						uint8_t op;
						r.u8(op);
						SOUP_IF_UNLIKELY (op != 0x41) // i32.const
						{
							return false;
						}
						int32_t value; r.soml(value);
						r.u8(op);
						SOUP_IF_UNLIKELY (op != 0x0b) // end
						{
							return false;
						}
						globals.emplace_back(value);
					}
				}
				break;

			case 7: // Export
				{
					size_t num_exports;
					r.oml(num_exports);
#if DEBUG_LOAD
					std::cout << num_exports << " export(s)\n";
#endif
					while (num_exports--)
					{
						size_t name_len;
						r.oml(name_len);
						std::string name;
						r.str(name_len, name);
#if DEBUG_LOAD
						std::cout << "- " << name << "\n";
#endif
						uint8_t kind; r.u8(kind);
						size_t index; r.oml(index);
						if (kind == 0) // function 
						{
							export_map.emplace(std::move(name), index);
						}
						// kind 2 = memory
					}
				}
				break;

			case 10: // Code
				{
					size_t num_functions;
					r.oml(num_functions);
#if DEBUG_LOAD
					std::cout << num_functions << " function(s)\n";
#endif
					while (num_functions--)
					{
						size_t body_size;
						r.oml(body_size);
						SOUP_IF_UNLIKELY (body_size == 0)
						{
							return false;
						}
						std::string body;
						r.str(body_size, body);
#if DEBUG_LOAD
						std::cout << "- " << string::bin2hex(body) << "\n";
#endif
						functions.emplace_back(std::move(body));
					}
				}
				break;

			case 11: // Data
				{
					size_t num_segments;
					r.oml(num_segments);
#if DEBUG_LOAD
					std::cout << num_segments << " data segment(s)\n";
#endif
					while (num_segments--)
					{
						r.skip(1); // flags
						uint8_t op;
						r.u8(op);
						SOUP_IF_UNLIKELY (op != 0x41) // i32.const
						{
							return false;
						}
						int32_t base; r.soml(base);
						r.u8(op);
						SOUP_IF_UNLIKELY (op != 0x0b) // end
						{
							return false;
						}
						size_t size; r.oml(size);
						SOUP_IF_UNLIKELY (base + size >= sizeof(memory))
						{
							return false;
						}
						r.raw(&memory[base], size);
					}
				}
				break;
			}
			if (section_size == 0)
			{
				r.oml(section_size);
			}
		}
		return true;
	}

	WasmScript::FunctionImport* WasmScript::getImportedFunction(const std::string& module_name, const std::string& function_name) noexcept
	{
		for (auto& fi : function_imports)
		{
			if (fi.function_name == function_name
				&& fi.module_name == module_name
				)
			{
				return &fi;
			}
		}
		return nullptr;
	}

	const std::string* WasmScript::getExportedFuntion(const std::string& name) const noexcept
	{
		if (auto e = export_map.find(name); e != export_map.end())
		{
			const size_t i = (e->second - function_imports.size());
			if (i < functions.size())
			{
				return &functions.at(i);
			}
		}
		return nullptr;
	}

	void WasmScript::linkWasiPreview1() noexcept
	{
		// https://github.com/bytecodealliance/wasmtime/blob/main/docs/WASI-tutorial.md#web-assembly-text-example
		if (auto fi = getImportedFunction("wasi_snapshot_preview1", "fd_write"))
		{
			fi->ptr = [](WasmVm& vm)
			{
				int32_t out_nwritten = vm.stack.top(); vm.stack.pop();
				int32_t iovs_len = vm.stack.top(); vm.stack.pop();
				int32_t iovs = vm.stack.top(); vm.stack.pop();
				int32_t file_descriptor = vm.stack.top(); vm.stack.pop();
				int32_t nwritten = 0;
				if (file_descriptor == 1) // stdout
				{
					while (iovs_len--)
					{
						int32_t iov_base = *vm.script.getMemory<int32_t>(iovs); iovs += 4;
						int32_t iov_len = *vm.script.getMemory<int32_t>(iovs); iovs += 4;
						fwrite(vm.script.getMemory<char>(iov_base), 1, iov_len, stdout);
						nwritten += iov_len;
					}
				}
				*vm.script.getMemory<int32_t>(out_nwritten) = nwritten;
				vm.stack.push(nwritten);
			};
		}
	}

	// WasmVm

	bool WasmVm::run(const std::string& data) SOUP_EXCAL
	{
		StringRefReader r(data);
		return run(r);
	}

	struct CtrlFlowEntry
	{
		size_t position;
		size_t stack_size;
	};

	static void skipOverBranch(Reader& r, size_t depth = 0) SOUP_EXCAL
	{
		uint8_t op;
		while (r.u8(op))
		{
			switch (op)
			{
			case 0x02: // block
			case 0x03: // loop
			case 0x04: // if
				r.skip(1); // void
				++depth;
				break;

			case 0x0b: // end
				if (depth == 0)
				{
					return;
				}
				--depth;
				break;

			case 0x0c: // br
			case 0x10: // call
			case 0x20: // local.get
			case 0x21: // local.set
			case 0x22: // local.tee
			case 0x23: // global.get
			case 0x24: // global.set
				{
					size_t imm;
					r.oml(imm);
				}
				break;

			case 0x28: // i32.load
			case 0x2d: // i32.load8_u
			case 0x2f: // i32.load16_u
			case 0x36: // i32.store
			case 0x3a: // i32.store8
			case 0x3b: // i32.store16
				{
					r.skip(1); // alignment
					int32_t offset; r.oml(offset);
				}
				break;

			case 0x41: // i32.const
				{
					int32_t value;
					r.soml(value);
				}
				break;
			}
		}
	}

	bool WasmVm::run(Reader& r) SOUP_EXCAL
	{
		size_t local_decl_count;
		r.oml(local_decl_count);
		while (local_decl_count--)
		{
			size_t type_count;
			r.oml(type_count);
			uint8_t type;
			r.u8(type);
			SOUP_IF_UNLIKELY (type != 0x7f) // i32
			{
#if DEBUG_VM
				std::cout << "Unsupported local type: " << (int)type << "\n";
#endif
				return false;
			}
			while (type_count--)
			{
				locals.emplace_back(0);
			}
		}

		std::stack<CtrlFlowEntry> ctrlflow{};

		uint8_t op;
		while (r.u8(op))
		{
			switch (op)
			{
			default:
#if DEBUG_VM
				std::cout << "Unsupported opcode: " << (int)op << "\n";
#endif
				return false;

			case 0x00: // unreachable
#if DEBUG_VM
				std::cout << "unreachable\n";
#endif
				return false;

			case 0x01: // nop
				break;

			case 0x02: // block
				r.skip(1); // void
				ctrlflow.emplace(CtrlFlowEntry{ (size_t)-1, stack.size()});
				break;

			case 0x03: // loop
				r.skip(1); // void
				ctrlflow.emplace(CtrlFlowEntry{ r.getPosition(), stack.size() });
#if DEBUG_VM
				std::cout << "loop at position " << r.getPosition() << " with stack size " << stack.size() << "\n";
#endif
				break;

			case 0x04: // if
				{
					r.skip(1); // void
					int32_t value = stack.top(); stack.pop();
					if (value)
					{
						ctrlflow.emplace(CtrlFlowEntry{ r.getPosition(), stack.size() });
					}
					else
					{
						skipOverBranch(r);
					}
				}
				break;

			case 0x0b: // end
				if (ctrlflow.empty())
				{
					return true;
				}
				ctrlflow.pop();
				break;

			case 0x0c: // br
				{
					SOUP_IF_UNLIKELY (ctrlflow.empty())
					{
#if DEBUG_VM
						std::cout << "branch despite empty ctrlflow stack\n";
#endif
						return false;
					}
					size_t depth;
					r.oml(depth);
#if DEBUG_VM
					std::cout << "unconditional branch with depth " << depth << ", new position = ";
#endif
					while (depth--)
					{
						ctrlflow.pop();
						SOUP_IF_UNLIKELY (ctrlflow.empty())
						{
#if DEBUG_VM
							std::cout << "branch depth exceeds ctrlflow stack\n";
#endif
							return false;
						}
					}
					if (ctrlflow.top().position == -1)
					{
						// branch forwards
						skipOverBranch(r, depth);
					}
					else
					{
						// branch backwards
						r.seek(ctrlflow.top().position);
					}
#if DEBUG_VM
					std::cout << r.getPosition() << "\n";
#endif
					while (stack.size() > ctrlflow.top().stack_size)
					{
						stack.pop();
					}
				}
				break;

			case 0x0f: // return
				return true;

			case 0x10: // call
				{
					size_t function_index;
					r.oml(function_index);
					SOUP_IF_UNLIKELY (function_index >= script.function_imports.size())
					{
#if DEBUG_VM
						std::cout << "call is out-of-bounds\n";
#endif
						return false;
					}
#if DEBUG_VM
					std::cout << "Calling into " << script.function_imports.at(function_index).module_name << ":" << script.function_imports.at(function_index).function_name << "\n";
#endif
					SOUP_IF_UNLIKELY (script.function_imports.at(function_index).ptr == nullptr)
					{
						return false;
					}
					script.function_imports.at(function_index).ptr(*this);
				}
				break;

			case 0x1a: // drop
				stack.pop();
				break;

			case 0x20: // local.get
				{
					size_t local_index;
					r.oml(local_index);
					SOUP_IF_UNLIKELY (local_index >= locals.size())
					{
#if DEBUG_VM
						std::cout << "local is out-of-bounds\n";
#endif
						return false;
					}
					stack.push(locals.at(local_index));
				}
				break;

			case 0x21: // local.set
				{
					size_t local_index;
					r.oml(local_index);
					SOUP_IF_UNLIKELY (local_index >= locals.size())
					{
#if DEBUG_VM
						std::cout << "local is out-of-bounds\n";
#endif
						return false;
					}
					locals.at(local_index) = stack.top(); stack.pop();
				}
				break;

			case 0x22: // local.tee
				{
					size_t local_index;
					r.oml(local_index);
					SOUP_IF_UNLIKELY (local_index >= locals.size())
					{
#if DEBUG_VM
						std::cout << "local is out-of-bounds\n";
#endif
						return false;
					}
					locals.at(local_index) = stack.top();
				}
				break;

			case 0x23: // global.get
				{
					size_t global_index;
					r.oml(global_index);
					SOUP_IF_UNLIKELY (global_index >= script.globals.size())
					{
#if DEBUG_VM
						std::cout << "global is out-of-bounds\n";
#endif
						return false;
					}
					stack.push(script.globals.at(global_index));
				}
				break;

			case 0x24: // global.set
				{
					size_t global_index;
					r.oml(global_index);
					SOUP_IF_UNLIKELY (global_index >= script.globals.size())
					{
#if DEBUG_VM
						std::cout << "global is out-of-bounds\n";
#endif
						return false;
					}
					script.globals.at(global_index) = stack.top(); stack.pop();
				}
				break;

			case 0x28: // i32.load
				{
					int32_t base = stack.top(); stack.pop();
					r.skip(1); // alignment
					int32_t offset; r.oml(offset);
					stack.emplace(*script.getMemory<int32_t>(base + offset));
				}
				break;

			case 0x2d: // i32.load8_u
				{
					int32_t base = stack.top(); stack.pop();
					r.skip(1); // alignment
					int32_t offset; r.oml(offset);
					stack.emplace(*script.getMemory<uint8_t>(base + offset));
				}
				break;

			case 0x2f: // i32.load16_u
				{
					int32_t base = stack.top(); stack.pop();
					r.skip(1); // alignment
					int32_t offset; r.oml(offset);
					stack.emplace(*script.getMemory<uint16_t>(base + offset));
				}
				break;

			case 0x36: // i32.store
				{
					int32_t value = stack.top(); stack.pop();
					int32_t base = stack.top(); stack.pop();
					r.skip(1); // alignment
					int32_t offset; r.oml(offset);
					*script.getMemory<int32_t>(base + offset) = value;
				}
				break;

			case 0x3a: // i32.store8
				{
					int32_t value = stack.top(); stack.pop();
					int32_t base = stack.top(); stack.pop();
					r.skip(1); // alignment
					int32_t offset; r.oml(offset);
					*script.getMemory<int8_t>(base + offset) = static_cast<int8_t>(value);
				}
				break;

			case 0x3b: // i32.store16
				{
					int32_t value = stack.top(); stack.pop();
					int32_t base = stack.top(); stack.pop();
					r.skip(1); // alignment
					int32_t offset; r.oml(offset);
					*script.getMemory<int16_t>(base + offset) = static_cast<int16_t>(value);
				}
				break;

			case 0x41: // i32.const
				{
					int32_t value;
					r.soml(value);
					stack.push(value);
				}
				break;

			case 0x45: // i32.eqz
				{
					int32_t value = stack.top(); stack.pop();
					stack.push(value == 0);
				}
				break;

			case 0x46: // i32.eq
				{
					int32_t b = stack.top(); stack.pop();
					int32_t a = stack.top(); stack.pop();
					stack.push(a == b);
				}
				break;

			case 0x47: // i32.ne
				{
					int32_t b = stack.top(); stack.pop();
					int32_t a = stack.top(); stack.pop();
					stack.push(a != b);
				}
				break;

			case 0x48: // i32.lt_s
				{
					int32_t b = stack.top(); stack.pop();
					int32_t a = stack.top(); stack.pop();
					stack.push(a < b);
				}
				break;

			case 0x49: // i32.lt_u
				{
					int32_t b = stack.top(); stack.pop();
					int32_t a = stack.top(); stack.pop();
					stack.push(static_cast<uint32_t>(a) < static_cast<uint32_t>(b));
				}
				break;

			case 0x4c: // i32.le_s
				{
					int32_t b = stack.top(); stack.pop();
					int32_t a = stack.top(); stack.pop();
					stack.push(a <= b);
				}
				break;

			case 0x4d: // i32.le_u
				{
					int32_t b = stack.top(); stack.pop();
					int32_t a = stack.top(); stack.pop();
					stack.push(static_cast<uint32_t>(a) <= static_cast<uint32_t>(b));
				}
				break;

			case 0x4a: // i32.gt_s
				{
					int32_t b = stack.top(); stack.pop();
					int32_t a = stack.top(); stack.pop();
					stack.push(a > b);
				}
				break;

			case 0x4b: // i32.gt_u
				{
					int32_t b = stack.top(); stack.pop();
					int32_t a = stack.top(); stack.pop();
					stack.push(static_cast<uint32_t>(a) > static_cast<uint32_t>(b));
				}
				break;

			case 0x4e: // i32.ge_s
				{
					int32_t b = stack.top(); stack.pop();
					int32_t a = stack.top(); stack.pop();
					stack.push(a >= b);
				}
				break;

			case 0x4f: // i32.ge_u
				{
					int32_t b = stack.top(); stack.pop();
					int32_t a = stack.top(); stack.pop();
					stack.push(static_cast<uint32_t>(a) >= static_cast<uint32_t>(b));
				}
				break;

			case 0x6a: // i32.add
				{
					int32_t b = stack.top(); stack.pop();
					int32_t a = stack.top(); stack.pop();
					stack.push(a + b);
				}
				break;

			case 0x6b: // i32.sub
				{
					int32_t b = stack.top(); stack.pop();
					int32_t a = stack.top(); stack.pop();
					stack.push(a - b);
				}
				break;

			case 0x6c: // i32.mul
				{
					int32_t b = stack.top(); stack.pop();
					int32_t a = stack.top(); stack.pop();
					stack.push(a * b);
				}
				break;

			case 0x6d: // i32.div_s
				{
					int32_t b = stack.top(); stack.pop();
					int32_t a = stack.top(); stack.pop();
					stack.push(a / b);
				}
				break;

			case 0x71: // i32.and
				{
					int32_t b = stack.top(); stack.pop();
					int32_t a = stack.top(); stack.pop();
					stack.push(a & b);
				}
				break;

			case 0x72: // i32.or
				{
					int32_t b = stack.top(); stack.pop();
					int32_t a = stack.top(); stack.pop();
					stack.push(a | b);
				}
				break;
			}
		}
		return true;
	}
}
