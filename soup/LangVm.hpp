#pragma once

#include "fwd.hpp"

#include <memory>
#include <stack>
#include <vector>

#include "Capture.hpp"
#include "Mixed.hpp"

namespace soup
{
	class LangVm
	{
	public:
		Reader& r;
		std::stack<std::shared_ptr<Mixed>> stack;
		std::unordered_map<std::string, std::shared_ptr<Mixed>> vars{};
		Capture cap;
	protected:
		uint8_t current_op;

	public:
		explicit LangVm(Reader& r, std::stack<std::shared_ptr<Mixed>>&& stack = {}) noexcept;
		~LangVm() noexcept;

		// for-loop paradigm

		[[nodiscard]] bool getNextOp(uint8_t& op);

		// OP-only paradigm

		using op_t = void(*)(LangVm&);
	protected:
		std::vector<op_t> ops{};
	public:
		void addOpcode(uint8_t opcode, op_t op);
		void execute();
		void assembleAndExecute();

		// Stack manipulation

		void push(Mixed&& val);
		void push(std::shared_ptr<Mixed> val);
		[[nodiscard]] std::shared_ptr<Mixed> popRaw();
		[[nodiscard]] std::shared_ptr<Mixed> pop();
		[[nodiscard]] std::shared_ptr<Mixed>& popVarRef();
		[[nodiscard]] std::string popVarName();
		[[nodiscard]] std::string popString();
		[[nodiscard]] StringReader popFunc();
	};
}
