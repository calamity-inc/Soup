#pragma once

#include "laBackend.hpp"

#include "fwd.hpp"

namespace soup
{
	class laWasmBackend : public laBackend
	{
	public:
		void linkPosix(irModule& m) final;
		[[nodiscard]] std::string compileModule(const irModule& m) const final;
		[[nodiscard]] static std::string getTypeSectionData(const irModule& m);
		[[nodiscard]] static std::string getImportSectionData(const irModule& m);
		[[nodiscard]] static std::string getFunctionSectionData(const irModule& m);
		[[nodiscard]] static std::string getMemorySectionData(const irModule& m);
		[[nodiscard]] static std::string getExportSectionData(const irModule& m);
		[[nodiscard]] static std::string getCodeSectionData(const irModule& m);
		[[nodiscard]] static std::string getDataSectionData(const irModule& m);
		[[nodiscard]] static std::string compileFunction(const irModule& m, const irFunction& fn);
	protected:
		static bool writeType(StringWriter& w, irType type);
		static int compileExpression(const irModule& m, StringWriter& w, const irExpression& e);
	};
}
