#pragma once

#include "fwd.hpp"

#include <memory>
#include <stack>

#include "Mixed.hpp"

namespace soup
{
	class LangVm
	{
	public:
		Reader* r;
		uint8_t current_op;
		std::stack<std::shared_ptr<Mixed>> stack{};
		std::unordered_map<std::string, std::shared_ptr<Mixed>> vars{};

		explicit LangVm(Reader* r) noexcept
			: r(r)
		{
		}

		[[nodiscard]] bool getNextOp(uint8_t& op);

		void push(Mixed&& val);
		void push(std::shared_ptr<Mixed> val);

	private:
		[[nodiscard]] std::shared_ptr<Mixed> popRaw();
	public:
		[[nodiscard]] std::shared_ptr<Mixed> pop();
		[[nodiscard]] std::shared_ptr<Mixed>& popVarRef();
		[[nodiscard]] std::string popString();
		[[nodiscard]] StringReader popFunc();
	};
}
