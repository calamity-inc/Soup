#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "fwd.hpp" // irFunction
#include "irType.hpp"
#include "UniquePtr.hpp"

NAMESPACE_SOUP
{
	enum irExpressionType : uint8_t
	{
		IR_CONST_BOOL,
		IR_CONST_I8,
		IR_CONST_I32,
		IR_CONST_I64,
		IR_CONST_PTR,

		IR_LOCAL_GET,
		IR_LOCAL_SET,
		IR_CALL,
		IR_RET,
		IR_IFELSE,
		IR_WHILE,
		IR_DISCARD,

		IR_ADD_I32,
		IR_ADD_I64,
		IR_ADD_PTR,
		IR_SUB_I32,
		IR_SUB_I64,
		//IR_MUL_I32,
		IR_MUL_I64,
		//IR_SDIV_I32,
		IR_SDIV_I64,
		//IR_UDIV_I32,
		IR_UDIV_I64,
		//IR_SMOD_I32,
		IR_SMOD_I64,
		//IR_UMOD_I32,
		IR_UMOD_I64,

		IR_EQUALS_I8,
		IR_EQUALS_I32,
		IR_EQUALS_I64,
		IR_NOTEQUALS_I8,
		IR_NOTEQUALS_I32,
		IR_NOTEQUALS_I64,

		IR_I64_TO_PTR, // zero-extend
		IR_I64_TO_I32,
		IR_I64_TO_I8,
		IR_I32_TO_I64_SX,
		IR_I32_TO_I64_ZX,
		IR_I8_TO_I64_SX,
		IR_I8_TO_I64_ZX,

		IR_LOAD_I8, // (ptr) -> (i8)
		IR_STORE_I8, // (ptr, i8) -> ()
		IR_STORE_I32, // (ptr, i32) -> ()
		IR_STORE_I64, // (ptr, i64) -> ()
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
				int8_t value;
			} const_i8;
			struct
			{
				int32_t value;
			} const_i32;
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
			} local_get, local_set;
			struct
			{
				intptr_t index;
			} call;
			struct
			{
				size_t ifinsns;
			} ifelse;
			struct
			{
				size_t count;
			} discard;
			uint64_t union_value; static_assert(sizeof(size_t) <= sizeof(uint64_t));
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

		bool optimiseByConstantFolding();

		[[nodiscard]] irType getResultType(const irFunction& fn) const noexcept;

		[[nodiscard]] bool isConstantZero() const noexcept;
		[[nodiscard]] bool isNegativeCompareToConstantZero() const noexcept;
		[[nodiscard]] bool isFoldableConstant() const noexcept;
		[[nodiscard]] UniquePtr<irExpression> clone() const;
		[[nodiscard]] UniquePtr<irExpression> inverted() const;
	};
}
