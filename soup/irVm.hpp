#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "irModule.hpp"
#include "irType.hpp"

NAMESPACE_SOUP
{
	// Although I think it's a bit weird to have a VM for IR, I think it does make sense to have a "ground truth", such that:
	// - An exception/assert in the IR VM would indicate a faulty frontend.
	// - Different results in IR VM vs compiled indicates a faulty... something (could be frontend, backend, or even the target CPU/VM).
	// This also allows for constant folding.
	struct irVm
	{
		struct Variable
		{
			union
			{
				bool b;
				int8_t i8;
				int32_t i32;
				int64_t i64;
				uint64_t ptr;
			} value;
			irType type;

			Variable(bool b) { value.b = b; type = IR_BOOL; }
			Variable(int8_t i8) { value.i8 = i8; type = IR_I8; }
			Variable(int32_t i32) { value.i32 = i32; type = IR_I32; }
			Variable(int64_t i64) { value.i64 = i64; type = IR_I64; }
			Variable(uint64_t ptr) { value.ptr = ptr; type = IR_PTR; }

			[[nodiscard]] static Variable fromArithmeticValue(int64_t value, irType type);

			[[nodiscard]] std::string toString() const noexcept;
		};

		std::string& memory;
		std::vector<Variable> locals{};

		irVm(std::string& memory) : memory(memory) {}

		std::vector<Variable> execute(const irModule& m, const irFunction& fn);
		std::vector<Variable> execute(const irModule& m, const irExpression& insn);
	};
}
