#pragma once

#include "RegexConstraint.hpp"

#include "RegexGroup.hpp"

namespace soup
{
	struct RegexGroupConstraint : public RegexConstraint
	{
		RegexGroup data;

		RegexGroupConstraint(const RegexGroup::ConstructorState& s, bool non_capturing)
			: data(s, non_capturing)
		{
		}

		[[nodiscard]] const RegexConstraintTransitionable* getEntrypoint() const noexcept final
		{
			return data.initial;
		}

		[[nodiscard]] std::string toString() const noexcept final
		{
			auto str = data.toString();
			if (data.isNonCapturing())
			{
				str.insert(0, "?:");
			}
			else if (!data.name.empty())
			{
				str.insert(0, 1, '\'');
				str.insert(0, data.name);
				str.insert(0, "?'");
			}
			str.insert(0, 1, '(');
			str.push_back(')');
			return str;
		}

		void getFlags(uint16_t& set, uint16_t& unset) const noexcept final
		{
			data.getFlags(set, unset);
		}

		[[nodiscard]] size_t getCursorAdvancement() const final
		{
			return data.getCursorAdvancement();
		}
	};
}
