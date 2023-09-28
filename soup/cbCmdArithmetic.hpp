#pragma once

#include "cbCmd.hpp"

#include "Bigint.hpp"

namespace soup
{
	struct cbCmdArithmetic : public cbCmd
	{
		[[nodiscard]] std::vector<std::string> getTriggers() const noexcept final
		{
			return { "+", "-", "*", "/" };
		}

		[[nodiscard]] cbResult process(cbParser& p) const noexcept final
		{
			auto ls = p.getArgNumericLefthand();
			auto rs = p.getArgNumeric();
			if (ls.empty() || rs.empty())
			{
				return "Malformed equation.";
			}
			auto l = Bigint::fromString(ls);
			auto r = Bigint::fromString(rs);
			if constexpr (true)
			{
				if (l.getNumBits() > 512
					|| l.getNumBits() > 512
					)
				{
					return "To avoid abuse, your arguments may not exceed 512 bits.";
				}
			}
			std::string msg = "That expression evaluates to ";
			msg.append(calculate(l, p.getTrigger().at(0), r).toString());
			msg.append(". :)");
			return cbResult(std::move(msg));
		}

		[[nodiscard]] static Bigint calculate(const Bigint& l, char op, const Bigint& r) noexcept
		{
			switch (op)
			{
			case '+': return l + r;
			case '-': return l - r;
			case '*': return l * r;
			case '/': return l / r;
			}
			// should be unreachable, but don't wanna throw
			return {};
		}
	};
}
