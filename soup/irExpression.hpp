#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "fwd.hpp" // irFunction
#include "irType.hpp"
#include "UniquePtr.hpp"

namespace soup
{
	enum irExpressionType : uint8_t
	{
		IR_CONST_BOOL,
		IR_CONST_I64,
		IR_CONST_PTR,

		IR_LOCAL_GET,
		IR_LOCAL_SET,
		IR_CALL,
		IR_RET,
		IR_WHILE,

		// Arithmetic Binary Operators: T, T -> T where T must not be bool.
		IR_ADD,
		IR_SUB,
		IR_MUL,
		IR_SDIV,
		IR_UDIV,

		IR_EQUALS, // T, T -> bool
		IR_NOTEQUALS, // T, T -> bool

		IR_READ_I8, // ptr -> i8

		IR_I64_TO_PTR, // zero-extend
		IR_I8_TO_I64_SX,
		IR_I8_TO_I64_ZX,
	};

	struct irExpression
	{
		irExpressionType type;
		union
		{
			struct
			{
				bool value;
			} const_bool;
			struct
			{
				int64_t value;
			} const_i64;
			struct
			{
				uint64_t value;
			} const_ptr;
			struct
			{
				uint32_t index;
			} local_get, local_set, call;
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

		[[nodiscard]] irType getResultType(const irFunction& fn) const noexcept;
	};
}
