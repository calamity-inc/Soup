#pragma once

#include "base.hpp"
#include "fwd.hpp"

#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

namespace soup
{
	class WasmScript
	{
	private:
		std::unordered_map<std::string, size_t> export_map{};
		std::vector<std::string> functions{};

	public:
		bool load(const std::string& data);
		bool load(Reader& r);

		[[nodiscard]] const std::string* getExportedFuntion(const std::string& name) const noexcept;
	};

	struct WasmVm
	{
		std::vector<int32_t> locals;
		std::stack<int32_t> stack;

		bool run(const std::string& data) SOUP_EXCAL;
		bool run(Reader& r) SOUP_EXCAL;
	};
}
