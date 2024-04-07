#pragma once

#include <string>
#include <vector>

#include "irFunction.hpp"

NAMESPACE_SOUP
{
	struct irModule
	{
		struct Data
		{
			std::string data{};
			uint8_t alignment;
			bool is_const;
		};

		struct FunctionImport
		{
			std::string module_name{};
			irFunction func;
		};

		std::vector<Data> data{};
		std::vector<FunctionImport> imports{};
		std::vector<irFunction> func_exports{};

		[[nodiscard]] std::string getContiguousMemory() const;
		[[nodiscard]] uint64_t allocateConstData(std::string&& data);
		[[nodiscard]] uint64_t allocateZeroedMemory(uint64_t size, uint8_t align = 1);

		bool optimiseByConstantFolding()
		{
			bool any_changes = false;
			for (auto& func : func_exports)
			{
				any_changes |= func.optimiseByConstantFolding();
			}
			return any_changes;
		}

		[[nodiscard]] irFunction& getFunction(intptr_t index);

		[[nodiscard]] intptr_t getFunctionIndex(const std::string& name);
		[[nodiscard]] intptr_t getStrlenFunctionIndex();
		[[nodiscard]] intptr_t getPrintFunctionIndex();

		[[nodiscard]] intptr_t getImportFunctionIndex(const std::string& mod, const std::string& func);
		[[nodiscard]] intptr_t getPosixWriteFunctionIndex();

		void updateCalls(intptr_t old_idx, intptr_t new_idx);
	};
}
