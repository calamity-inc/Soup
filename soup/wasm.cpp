#include "wasm.hpp"

#include "Reader.hpp"
#include "StringRefReader.hpp"

#define DEBUG_LOAD false

#if DEBUG_LOAD
#include <iostream>
#include "string.hpp"
#endif

namespace soup
{
	// WasmScript

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
				if (section_size == 0)
				{
					return false;
				}
				r.skip(section_size);
				break;

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
						if (body_size == 0)
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
			}
			if (section_size == 0)
			{
				r.oml(section_size);
			}
		}
		return true;
	}

	const std::string* WasmScript::getExportedFuntion(const std::string& name) const noexcept
	{
		if (auto e = export_map.find(name); e != export_map.end())
		{
			if (e->second < functions.size())
			{
				return &functions.at(e->second);
			}
		}
		return nullptr;
	}

	// WasmVm

	bool WasmVm::run(const std::string& data) SOUP_EXCAL
	{
		StringRefReader r(data);
		return run(r);
	}

	bool WasmVm::run(Reader& r) SOUP_EXCAL
	{
		size_t local_decl_count;
		r.oml(local_decl_count);
		while (local_decl_count--)
		{
			size_t type_count;
			r.oml(type_count);
			r.skip(type_count);
		}

		uint8_t op;
		while (r.u8(op))
		{
			switch (op)
			{
			default:
				return false;

			case 0x0b: // end
				return true;

			case 0x20: // local.get
				{
					size_t local_index;
					r.oml(local_index);
					SOUP_IF_UNLIKELY (local_index >= locals.size())
					{
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
						return false;
					}
					locals.at(local_index) = stack.top(); stack.pop();
				}
				break;

			case 0x41: // i32.const
				{
					int32_t value;
					r.oml(value);
					stack.push(value);
				}
				break;

			case 0x46: // i32.eq
				{
					int32_t a = stack.top(); stack.pop();
					int32_t b = stack.top(); stack.pop();
					stack.push(a == b);
				}
				break;

			case 0x6a: // i32.add
				{
					int32_t a = stack.top(); stack.pop();
					int32_t b = stack.top(); stack.pop();
					stack.push(a + b);
				}
				break;

			case 0x6b: // i32.sub
				{
					int32_t a = stack.top(); stack.pop();
					int32_t b = stack.top(); stack.pop();
					stack.push(a - b);
				}
				break;

			case 0x6c: // i32.mul
				{
					int32_t a = stack.top(); stack.pop();
					int32_t b = stack.top(); stack.pop();
					stack.push(a * b);
				}
				break;

			case 0x6d: // i32.div_s
				{
					int32_t a = stack.top(); stack.pop();
					int32_t b = stack.top(); stack.pop();
					stack.push(a / b);
				}
				break;

			case 0x71: // i32.and
				{
					int32_t a = stack.top(); stack.pop();
					int32_t b = stack.top(); stack.pop();
					stack.push(a & b);
				}
				break;

			case 0x72: // i32.or
				{
					int32_t a = stack.top(); stack.pop();
					int32_t b = stack.top(); stack.pop();
					stack.push(a | b);
				}
				break;
			}
		}
		return true;
	}
}
