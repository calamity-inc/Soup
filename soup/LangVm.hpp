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
		std::stack<std::shared_ptr<Mixed>> stack;
		std::unordered_map<std::string, std::shared_ptr<Mixed>> vars{};
	protected:
		uint8_t current_op;

	public:
		explicit LangVm(Reader* r, std::stack<std::shared_ptr<Mixed>>&& stack = {}) noexcept;
		~LangVm() noexcept;

		[[nodiscard]] bool getNextOp(uint8_t& op);

		void push(Mixed&& val);
		void push(std::shared_ptr<Mixed> val);

	public:
		[[nodiscard]] std::shared_ptr<Mixed> popRaw();
		[[nodiscard]] std::shared_ptr<Mixed> pop();
		[[nodiscard]] std::shared_ptr<Mixed>& popVarRef();
		[[nodiscard]] std::string popVarName();
		[[nodiscard]] std::string popString();
		[[nodiscard]] StringReader popFunc();
	};
}
