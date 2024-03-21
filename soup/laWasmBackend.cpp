#include "laWasmBackend.hpp"

#include "StringWriter.hpp"

namespace soup
{
	std::string laWasmBackend::compileModule(const irModule& m) const
	{
		StringWriter w;

		uint8_t b; uint32_t u;
		u = 1836278016; w.u32_le(u);
		u = 1; w.u32_le(u);

		// Type section
		{
			b = 1; w.u8(b);
			auto data = getTypeSectionData(m);
			w.oml(data.size());
			w.raw(data.data(), data.size());
		}

		// Function section
		{
			b = 3; w.u8(b);
			auto data = getFunctionSectionData(m);
			w.oml(data.size());
			w.raw(data.data(), data.size());
		}

		// Export section
		{
			b = 7; w.u8(b);
			auto data = getExportSectionData(m);
			w.oml(data.size());
			w.raw(data.data(), data.size());
		}

		// Code section
		{
			b = 10; w.u8(b);
			auto data = getCodeSectionData(m);
			w.oml(data.size());
			w.raw(data.data(), data.size());
		}

		SOUP_MOVE_RETURN(w.data);
	}

	std::string laWasmBackend::getTypeSectionData(const irModule& m)
	{
		StringWriter w;
		w.oml(m.func_exports.size());
		for (const auto& func : m.func_exports)
		{
			uint8_t type_type = 0x60; w.u8(type_type); // function
			w.oml(func.parameters.size());
			for (const auto& t : func.parameters)
			{
				writeType(w, t);
			}
			w.oml(func.returns.size());
			for (const auto& t : func.returns)
			{
				writeType(w, t);
			}
		}
		SOUP_MOVE_RETURN(w.data);
	}

	std::string laWasmBackend::getFunctionSectionData(const irModule& m)
	{
		StringWriter w;
		w.oml(m.func_exports.size());
		for (uint32_t i = 0; i != m.func_exports.size(); ++i)
		{
			w.oml(i); // We're keeping function & type indecies the same for now for simplicity.
		}
		SOUP_MOVE_RETURN(w.data);
	}

	std::string laWasmBackend::getExportSectionData(const irModule& m)
	{
		StringWriter w;
		w.oml(m.func_exports.size());
		for (uint32_t i = 0; i != m.func_exports.size(); ++i)
		{
			const auto& func = m.func_exports[i];
			w.oml(func.name.size());
			w.str(func.name.size(), func.name.data());
			uint8_t kind = 0; w.u8(kind); // function
			w.oml(i);
		}
		SOUP_MOVE_RETURN(w.data);
	}

	std::string laWasmBackend::getCodeSectionData(const irModule& m)
	{
		StringWriter w;
		w.oml(m.func_exports.size());
		for (const auto& func : m.func_exports)
		{
			auto body = compileFunction(m, func.insns);
			w.oml(body.size());
			w.str(body.size(), body.data());
		}
		SOUP_MOVE_RETURN(w.data);
	}

	bool laWasmBackend::writeType(StringWriter& w, irType type)
	{
		uint8_t b;
		switch (type)
		{
		case IR_I64: b = 0x7e; break;
		}
		return w.u8(b);
	}

	std::string laWasmBackend::compileFunction(const irModule& m, const std::vector<UniquePtr<irExpression>>& insns)
	{
		StringWriter w;
		w.skip(1); // locals
		for (const auto& insn : insns)
		{
			int ret = compileExpression(m, w, *insn);
			if (insn->type == IR_RET)
			{
				break;
			}
			while (ret--)
			{
				uint8_t b = 0x1a; w.u8(b); // drop
			}
		}
		uint8_t b = 0x0b; w.u8(b); // end
		SOUP_MOVE_RETURN(w.data);
	}

	int laWasmBackend::compileExpression(const irModule& m, StringWriter& w, const irExpression& e)
	{
		uint8_t b;
		switch (e.type)
		{
		case IR_CONST_I64:
			b = 0x42; w.u8(b); // i64.const
			w.soml(e.constant_value);
			return 1;

		case IR_LOCAL:
			b = 0x20; w.u8(b); // local.get
			w.oml(e.index);
			return 1;

		case IR_CALL:
			for (const auto& child : e.children)
			{
				compileExpression(m, w, *child);
			}
			b = 0x10; w.u8(b); // call
			w.oml(e.index);
			return static_cast<int>(m.func_exports.at(e.index).returns.size());

		case IR_ADD:
			SOUP_ASSERT(e.children.size() == 2);
			SOUP_ASSERT(compileExpression(m, w, *e.children[0]) == 1);
			SOUP_ASSERT(compileExpression(m, w, *e.children[1]) == 1);
			b = 0x7c; w.u8(b); // i64.add
			return 1;

		case IR_SUB:
			SOUP_ASSERT(e.children.size() == 2);
			SOUP_ASSERT(compileExpression(m, w, *e.children[0]) == 1);
			SOUP_ASSERT(compileExpression(m, w, *e.children[1]) == 1);
			b = 0x7d; w.u8(b); // i64.sub
			return 1;

		case IR_MUL:
			SOUP_ASSERT(e.children.size() == 2);
			SOUP_ASSERT(compileExpression(m, w, *e.children[0]) == 1);
			SOUP_ASSERT(compileExpression(m, w, *e.children[1]) == 1);
			b = 0x7e; w.u8(b); // i64.mul
			return 1;

		case IR_SDIV:
			SOUP_ASSERT(e.children.size() == 2);
			SOUP_ASSERT(compileExpression(m, w, *e.children[0]) == 1);
			SOUP_ASSERT(compileExpression(m, w, *e.children[1]) == 1);
			b = 0x7f; w.u8(b); // i64.div_s
			return 1;

		case IR_UDIV:
			SOUP_ASSERT(e.children.size() == 2);
			SOUP_ASSERT(compileExpression(m, w, *e.children[0]) == 1);
			SOUP_ASSERT(compileExpression(m, w, *e.children[1]) == 1);
			b = 0x80; w.u8(b); // i64.div_u
			return 1;

		case IR_RET:
			for (const auto& child : e.children)
			{
				compileExpression(m, w, *child);
			}
			return static_cast<int>(e.children.size());
		}
		SOUP_UNREACHABLE;
	}
}
