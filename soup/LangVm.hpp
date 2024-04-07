#pragma once

#include "fwd.hpp"

#include <stack>
#include <vector>

#include "Capture.hpp"
#include "Mixed.hpp"
#include "SharedPtr.hpp"

NAMESPACE_SOUP
{
	class LangVm
	{
	public:
		Reader& r;
		std::stack<SharedPtr<Mixed>> stack;
		std::unordered_map<std::string, SharedPtr<Mixed>> vars{};
		Capture cap;
	protected:
		uint8_t current_op;

	public:
		explicit LangVm(Reader& r, std::stack<SharedPtr<Mixed>>&& stack = {}) noexcept;
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
		void push(SharedPtr<Mixed> val);
		[[nodiscard]] SharedPtr<Mixed> popRaw();
		[[nodiscard]] SharedPtr<Mixed> pop();
		[[nodiscard]] SharedPtr<Mixed>& popVarRef();
		[[nodiscard]] std::string popVarName();
		[[nodiscard]] std::string popString();
		[[nodiscard]] StringReader popFunc();
	};
}
