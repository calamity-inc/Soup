#pragma once

#include <string>
#include <vector>

#include "irFunction.hpp"

namespace soup
{
	struct irModule
	{
		struct FunctionImport
		{
			std::string module_name{};
			irFunction func;
		};

		std::string data{};
		std::vector<FunctionImport> imports{};
		std::vector<irFunction> func_exports{};

		[[nodiscard]] uint32_t getFunctionIndex(const std::string& name);
		[[nodiscard]] uint32_t getStrlenFunctionIndex();
		[[nodiscard]] uint32_t getPrintFunctionIndex();

		[[nodiscard]] int64_t getImportFunctionIndex(const std::string& mod, const std::string& func);
		[[nodiscard]] int64_t getPosixWriteFunctionIndex();

		void updateCalls(int64_t old_idx, int64_t new_idx);
	};
}
