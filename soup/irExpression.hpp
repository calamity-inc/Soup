#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "UniquePtr.hpp"

namespace soup
{
	enum irExpressionType : uint8_t
	{
		IR_CONST_I64,
		IR_LOCAL,
		IR_CALL,

		// Binary Operators (2 -> 1)
		IR_ADD,
		IR_SUB,
		IR_MUL,
		IR_SDIV,
		IR_UDIV,

		IR_RET,
	};

	struct irExpression
	{
		irExpressionType type;
		union
		{
			int64_t constant_value;
			uint32_t index; // IR_LOCAL, IR_CALL
		};
		std::vector<UniquePtr<irExpression>> children{};

		irExpression() = delete;

		irExpression(irExpressionType type)
			: type(type)
		{
		}

		irExpression(irExpressionType type, UniquePtr<irExpression>&& child)
			: type(type)
		{
			children.emplace_back(std::move(child));
		}

		[[nodiscard]] std::string toString(unsigned int depth = 0) const noexcept;
	};
}
