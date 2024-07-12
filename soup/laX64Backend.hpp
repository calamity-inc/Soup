#pragma once

#include "laBackend.hpp"

#include "x64.hpp"

NAMESPACE_SOUP
{
	// Work in progress, hence not extending laBackend yet.
	struct laX64Backend
	{
		[[nodiscard]] static std::string compileFunction(const irModule& m, const irFunction& fn);
		static int compileExpression(const irModule& m, StringWriter& w, const irExpression& e, const irExpression* outgoing);
		static void discard(StringWriter& w, int nres);
		[[nodiscard]] static x64Register getIncomingRegister(const irExpression& e);
	};
}
