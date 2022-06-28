#pragma once

#include "fwd.hpp"

#include <stack>

#include "Mixed.hpp"

namespace soup
{
	class LangVm
	{
	public:
		struct Val : public Mixed
		{
			bool is_func = false;

			using Mixed::Mixed;

			[[nodiscard]] constexpr bool isFunc() const
			{
				return is_func;
			}

			[[nodiscard]] const char* getTypeName() const
			{
				if (is_func)
				{
					return "function";
				}
				if (isUInt())
				{
					return "var ref";
				}
				return Mixed::getTypeName();
			}

			[[nodiscard]] std::string& getString() const;
			[[nodiscard]] std::string& getFunc() const;
		};

		Reader* r;
		uint8_t current_op;
		std::stack<Val> stack{};
		std::unordered_map<uint8_t, Val> vars{};

		explicit LangVm(Reader* r) noexcept
			: r(r)
		{
		}

		[[nodiscard]] bool getNextOp(uint8_t& op);

		void push(Val&& val);
	private:
		[[nodiscard]] Val popRaw();
	public:
		[[nodiscard]] Val pop();
		[[nodiscard]] Val& popVarRef();
		[[nodiscard]] std::string popString();
		[[nodiscard]] StringReader popFunc();
	};
}
