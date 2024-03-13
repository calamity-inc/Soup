#pragma once

#include "base.hpp"
#include "fwd.hpp"

#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

namespace soup
{
	struct WasmVm;

	using wasm_ffi_func_t = void(*)(WasmVm&);

	struct WasmScript
	{
		struct FunctionType
		{
			size_t num_parameters;
			size_t num_results;
		};

		struct FunctionImport
		{
			std::string module_name;
			std::string function_name;
			wasm_ffi_func_t ptr;
		};

		uint8_t* memory = nullptr;
		size_t memory_size = 0;
		std::vector<size_t> functions{};
		std::vector<FunctionType> types{};
		std::vector<FunctionImport> function_imports{};
		std::vector<int32_t> globals{};
		std::unordered_map<std::string, size_t> export_map{};
		std::vector<std::string> code{};

		~WasmScript() noexcept;

		bool load(const std::string& data);
		bool load(Reader& r);

		[[nodiscard]] FunctionImport* getImportedFunction(const std::string& module_name, const std::string& function_name) noexcept;
		[[nodiscard]] const std::string* getExportedFuntion(const std::string& name) const noexcept;

		template <typename T>
		[[nodiscard]] T* getMemory(int32_t ptr) noexcept
		{
			SOUP_IF_UNLIKELY (ptr < 0 || ptr >= memory_size)
			{
				return nullptr;
			}
			return (T*)&memory[ptr];
		}

		void linkWasiPreview1() noexcept;
	};

	struct WasmVm
	{
		std::stack<int32_t> stack;
		std::vector<int32_t> locals;
		WasmScript& script;

		WasmVm(WasmScript& script)
			: script(script)
		{
		}

		bool run(const std::string& data) SOUP_EXCAL;
		bool run(Reader& r) SOUP_EXCAL;
	};
}
