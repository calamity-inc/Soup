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
	public:
		char memory[0x10'000];

	private:
		std::unordered_map<std::string, size_t> export_map{};
		std::vector<std::string> functions{};

	public:
		WasmScript();

		bool load(const std::string& data);
		bool load(Reader& r);

		[[nodiscard]] const std::string* getExportedFuntion(const std::string& name) const noexcept;

		template <typename T>
		[[nodiscard]] T* getMemory(int32_t ptr)
		{
			return (T*)&memory[ptr];
		}
	};

	struct WasmVm
	{
		std::stack<int32_t> stack;
		std::vector<int32_t> locals;
		WasmScript& script;

		WasmVm(WasmScript& script)
			: script(script)
		{
		}

		bool run(const std::string& data) SOUP_EXCAL;
		bool run(Reader& r) SOUP_EXCAL;
	};
}
