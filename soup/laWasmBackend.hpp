#pragma once

#include "laBackend.hpp"

#include "fwd.hpp"

namespace soup
{
	class laWasmBackend : public laBackend
	{
	public:
		[[nodiscard]] std::string compileModule(const irModule& m) const final;
		[[nodiscard]] static std::string getTypeSectionData(const irModule& m);
		[[nodiscard]] static std::string getFunctionSectionData(const irModule& m);
		[[nodiscard]] static std::string getMemorySectionData(const irModule& m);
		[[nodiscard]] static std::string getExportSectionData(const irModule& m);
		[[nodiscard]] static std::string getCodeSectionData(const irModule& m);
		[[nodiscard]] static std::string getDataSectionData(const irModule& m);
		[[nodiscard]] static std::string compileFunction(const irModule& m, const std::vector<UniquePtr<irExpression>>& insns);
	protected:
		static bool writeType(StringWriter& w, irType type);
		static int compileExpression(const irModule& m, StringWriter& w, const irExpression& e);
	};
}
