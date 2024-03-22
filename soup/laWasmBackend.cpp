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

		// Memory section
		{
			b = 5; w.u8(b);
			auto data = getMemorySectionData(m);
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

		// Data section
		if (!m.data.empty())
		{
			b = 11; w.u8(b);
			auto data = getDataSectionData(m);
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

	std::string laWasmBackend::getMemorySectionData(const irModule& m)
	{
		StringWriter w;
		w.oml(1); // num memories
		w.skip(1); // flags
		w.oml(1); // num pages
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
			auto body = compileFunction(m, func);
			w.oml(body.size());
			w.str(body.size(), body.data());
		}
		SOUP_MOVE_RETURN(w.data);
	}

	std::string laWasmBackend::getDataSectionData(const irModule& m)
	{
		StringWriter w;
		uint8_t b;
		w.oml(1); // num segments
		w.skip(1); // flags
		b = 0x41; w.u8(b); // i32.const
		w.soml(0); // base
		b = 0x0b; w.u8(b); // end
		w.oml(m.data.size());
		w.str(m.data.size(), m.data.data());
		SOUP_MOVE_RETURN(w.data);
	}

	bool laWasmBackend::writeType(StringWriter& w, irType type)
	{
		uint8_t b;
		switch (type)
		{
		case IR_BOOL: b = 0x7f; break; // i32
		case IR_I8: b = 0x7f; break; // i32
		case IR_I64: b = 0x7e; break; // i64
		case IR_PTR: b = 0x7f; break; // i32
		}
		return w.u8(b);
	}

	std::string laWasmBackend::compileFunction(const irModule& m, const irFunction& fn)
	{
		StringWriter w;
		w.oml(fn.locals.size());
		for (const auto& local : fn.locals)
		{
			w.oml(1);
			writeType(w, local);
		}
		for (const auto& insn : fn.insns)
		{
			int ret = compileExpression(m, fn, w, *insn);
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

	int laWasmBackend::compileExpression(const irModule& m, const irFunction& fn, StringWriter& w, const irExpression& e)
	{
		uint8_t b;
		switch (e.type)
		{
		case IR_CONST_BOOL:
			b = 0x41; w.u8(b); // i32.const
			w.soml(e.constant_value);
			return 1;

		case IR_CONST_I64:
			b = 0x42; w.u8(b); // i64.const
			w.soml(e.constant_value);
			return 1;

		case IR_CONST_PTR:
			b = 0x41; w.u8(b); // i32.const
			w.soml(e.constant_value);
			return 1;

		case IR_LOCAL_GET:
			b = 0x20; w.u8(b); // local.get
			w.oml(e.index);
			return 1;

		case IR_LOCAL_SET:
			compileExpression(m, fn, w, *e.children.at(0));
			b = 0x21; w.u8(b); // local.set
			w.oml(e.index);
			return 0;

		case IR_CALL:
			for (const auto& child : e.children)
			{
				compileExpression(m, fn, w, *child);
			}
			b = 0x10; w.u8(b); // call
			w.oml(e.index);
			return static_cast<int>(m.func_exports.at(e.index).returns.size());

		case IR_RET:
			for (const auto& child : e.children)
			{
				compileExpression(m, fn, w, *child);
			}
			return static_cast<int>(e.children.size());

		case IR_WHILE:
			b = 0x03; w.u8(b); // loop
			b = 0x40; w.u8(b); // void
			SOUP_ASSERT(compileExpression(m, fn, w, *e.children.at(0)) == 1);
			b = 0x04; w.u8(b); // if
			b = 0x40; w.u8(b); // void
			for (size_t i = 1; i != e.children.size(); ++i)
			{
				int ret = compileExpression(m, fn, w, *e.children[i]);
				while (ret--)
				{
					uint8_t b = 0x1a; w.u8(b); // drop
				}
			}
			b = 0x0c; w.u8(b); // br
			b = 0x01; w.u8(b); // skip over 'if'; back to 'loop'
			b = 0x0b; w.u8(b); // end
			b = 0x0b; w.u8(b); // end
			return 0;

		case IR_ADD:
			SOUP_ASSERT(e.children.size() == 2);
			SOUP_ASSERT(compileExpression(m, fn, w, *e.children[0]) == 1);
			SOUP_ASSERT(compileExpression(m, fn, w, *e.children[1]) == 1);
			if (e.children[0]->getResultType(fn) == IR_PTR)
			{
				b = 0x6a; w.u8(b); // i32.add
			}
			else
			{
				b = 0x7c; w.u8(b); // i64.add
			}
			return 1;

		case IR_SUB:
			SOUP_ASSERT(e.children.size() == 2);
			SOUP_ASSERT(compileExpression(m, fn, w, *e.children[0]) == 1);
			SOUP_ASSERT(compileExpression(m, fn, w, *e.children[1]) == 1);
			if (e.children[0]->getResultType(fn) == IR_PTR)
			{
				b = 0x6b; w.u8(b); // i32.sub
			}
			else
			{
				b = 0x7d; w.u8(b); // i64.sub
			}
			return 1;

		case IR_MUL:
			SOUP_ASSERT(e.children.size() == 2);
			SOUP_ASSERT(compileExpression(m, fn, w, *e.children[0]) == 1);
			SOUP_ASSERT(compileExpression(m, fn, w, *e.children[1]) == 1);
			b = 0x7e; w.u8(b); // i64.mul
			return 1;

		case IR_SDIV:
			SOUP_ASSERT(e.children.size() == 2);
			SOUP_ASSERT(compileExpression(m, fn, w, *e.children[0]) == 1);
			SOUP_ASSERT(compileExpression(m, fn, w, *e.children[1]) == 1);
			b = 0x7f; w.u8(b); // i64.div_s
			return 1;

		case IR_UDIV:
			SOUP_ASSERT(e.children.size() == 2);
			SOUP_ASSERT(compileExpression(m, fn, w, *e.children[0]) == 1);
			SOUP_ASSERT(compileExpression(m, fn, w, *e.children[1]) == 1);
			b = 0x80; w.u8(b); // i64.div_u
			return 1;

		case IR_EQUALS:
			SOUP_ASSERT(e.children.size() == 2);
			SOUP_ASSERT(compileExpression(m, fn, w, *e.children[0]) == 1);
			SOUP_ASSERT(compileExpression(m, fn, w, *e.children[1]) == 1);
			b = 0x51; w.u8(b); // i64.eq
			return 1;

		case IR_NOTEQUALS:
			SOUP_ASSERT(e.children.size() == 2);
			SOUP_ASSERT(compileExpression(m, fn, w, *e.children[0]) == 1);
			SOUP_ASSERT(compileExpression(m, fn, w, *e.children[1]) == 1);
			b = 0x52; w.u8(b); // i64.ne
			return 1;

		case IR_READ_I8:
			SOUP_ASSERT(compileExpression(m, fn, w, *e.children.at(0)) == 1);
			b = 0x2d; w.u8(b); // i32.load8_u
			w.skip(2); // memflags + offset
			return 1;

		case IR_I64_TO_PTR:
			SOUP_ASSERT(compileExpression(m, fn, w, *e.children.at(0)) == 1);
			b = 0xa7; w.u8(b); // i32.wrap_i64
			return 1;

		case IR_I8_TO_I64_SX:
			// Possible optimisation: Detect child being IR_READ_I8 and emit directly as i64.load8_s.
			SOUP_ASSERT(compileExpression(m, fn, w, *e.children.at(0)) == 1);
			/* First, sign-extend i8 to i32: */ b = 0xc0; w.u8(b); // i32.extend8_s
			/* Then, sign-extend i32 to i64: */ b = 0xac; w.u8(b); // i64.extend_i32_s
			return 1;

		case IR_I8_TO_I64_ZX:
			// Possible optimisation: Detect child being IR_READ_I8 and emit directly as i64.load8_u.
			SOUP_ASSERT(compileExpression(m, fn, w, *e.children.at(0)) == 1);
			/* First, zero-extend i8 to i32: IR_READ_I8 would have left the top bits all zero, so nothing to do. */
			/* Then, zero-extend i32 to i64: */ b = 0xad; w.u8(b); // i64.extend_i32_u
			return 1;
		}
		SOUP_UNREACHABLE;
	}
}
