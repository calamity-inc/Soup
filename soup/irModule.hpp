#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "irExpression.hpp"

namespace soup
{
	enum irType : uint8_t
	{
		IR_I64,
		IR_PTR,
	};

	struct irModule
	{
		struct FuncExport
		{
			std::string name;
			std::vector<irType> parameters{};
			std::vector<irType> returns{};
			std::vector<UniquePtr<irExpression>> insns{};
		};

		std::string data{};
		std::vector<FuncExport> func_exports{};
	};
}
