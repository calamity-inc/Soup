#include "RegexGroup.hpp"

#include "Regex.hpp"
#include "RegexAnyCharConstraint.hpp"
#include "RegexCharConstraint.hpp"
#include "RegexEndConstraint.hpp"
#include "RegexGreedyOneConstraint.hpp"
#include "RegexGroupConstraint.hpp"
#include "RegexOptConstraint.hpp"
#include "RegexRangeConstraint.hpp"
#include "RegexStartConstraint.hpp"

namespace soup
{
	struct TransitionsVector
	{
		std::vector<const RegexConstraintTransitionable**> data;
		std::vector<const RegexConstraintTransitionable**> prev_data;

		void emplace(const RegexConstraintTransitionable** p)
		{
			data.emplace_back(p);
		}

		void setPreviousTransitionTo(const RegexConstraintTransitionable* c) noexcept
		{
			for (const auto& p : prev_data)
			{
				*p = c;
			}
		}

		void setTransitionTo(const RegexConstraintTransitionable* c) noexcept
		{
			for (const auto& p : data)
			{
				*p = c;
			}
			prev_data = std::move(data);
			data.clear();
		}

		void discharge(std::vector<const RegexConstraintTransitionable**>& outTransitions) noexcept
		{
			for (const auto& p : data)
			{
				outTransitions.emplace_back(p);
			}
			data.clear();
		}
	};

	RegexGroup::RegexGroup(const ConstructorState& s, bool non_capturing)
		: index(non_capturing ? -1 : s.next_index++)
	{
		TransitionsVector success_transitions;
		success_transitions.data = { &initial };

		RegexAlternative a{};

		std::vector<const RegexConstraintTransitionable**> alternatives_transitions{};

		bool escape = false;
		for (; s.it != s.end; ++s.it)
		{
			if (escape)
			{
				escape = false;
			}
			else
			{
				if (*s.it == '\\')
				{
					escape = true;
					continue;
				}
				else if (*s.it == '|')
				{
					alternatives.emplace_back(std::move(a));
					a.constraints.clear();
					success_transitions.discharge(alternatives_transitions);
					continue;
				}
				else if (*s.it == '(')
				{
					bool non_capturing = false;
					std::string name{};
					if (++s.it != s.end && *s.it == '?')
					{
						if (++s.it != s.end)
						{
							if (*s.it == ':')
							{
								++s.it;
								non_capturing = true;
							}
							else if (*s.it == '\'')
							{
								while (++s.it != s.end && *s.it != '\'')
								{
									name.push_back(*s.it);
								}
								if (s.it != s.end)
								{
									++s.it;
								}
							}
						}
					}
					auto upGC = soup::make_unique<RegexGroupConstraint>(s, non_capturing);
					upGC->group.parent = this;
					upGC->group.name = std::move(name);
					success_transitions.setTransitionTo(upGC->group.initial);
					success_transitions.data = std::move(s.alternatives_transitions);
					a.constraints.emplace_back(std::move(upGC));
					if (s.it == s.end)
					{
						break;
					}
					continue;
				}
				else if (*s.it == ')')
				{
					break;
				}
				else if (*s.it == '+')
				{
					UniquePtr<RegexConstraint> upModifiedConstraint = std::move(a.constraints.back());
					auto pModifiedConstraint = upModifiedConstraint.get();
					auto upGreedyConstraint = soup::make_unique<RegexGreedyOneConstraint>(std::move(upModifiedConstraint));

					pModifiedConstraint->group = this;

					// constraint --[success]-> greedy
					success_transitions.setTransitionTo(upGreedyConstraint.get());

					// greedy --[success]-> constraint
					upGreedyConstraint->success_transition = pModifiedConstraint->getTransition();

					// greedy --[rollback]-> next-constraint
					success_transitions.emplace(&upGreedyConstraint->rollback_transition);

					// If we don't have a next constraint, rollback is match success.
					*reinterpret_cast<uintptr_t*>(&upGreedyConstraint->rollback_transition) = 1;

					a.constraints.back() = std::move(upGreedyConstraint);
					continue;
				}
				else if (*s.it == '?')
				{
					UniquePtr<RegexConstraint> upModifiedConstraint = std::move(a.constraints.back());
					auto pModifiedConstraint = upModifiedConstraint.get();
					auto upOptConstraint = soup::make_unique<RegexOptConstraint>(std::move(upModifiedConstraint));

					pModifiedConstraint->group = this;

					// prev-constraint --[success]-> opt
					success_transitions.setPreviousTransitionTo(upOptConstraint.get());

					// opt --[success]-> constraint
					upOptConstraint->success_transition = pModifiedConstraint->getTransition();

					// opt --[rollback]-> next-constraint
					success_transitions.emplace(&upOptConstraint->rollback_transition);

					// If we don't have a next constraint, rollback is match success.
					*reinterpret_cast<uintptr_t*>(&upOptConstraint->rollback_transition) = 1;

					a.constraints.back() = std::move(upOptConstraint);
					continue;
				}
				else if (*s.it == '.')
				{
					UniquePtr<RegexConstraintTransitionable> upC;
					if (s.flags & Regex::single_line)
					{
						upC = soup::make_unique<RegexAnyCharConstraint<true>>();
					}
					else
					{
						upC = soup::make_unique<RegexAnyCharConstraint<false>>();
					}
					auto pC = upC.get();
					a.constraints.emplace_back(std::move(upC));
					success_transitions.setTransitionTo(pC);
					success_transitions.emplace(&pC->success_transition);
					continue;
				}
				else if (*s.it == '[')
				{
					auto upC = soup::make_unique<RegexRangeConstraint>(s.it, s.end);
					auto pC = upC.get();
					a.constraints.emplace_back(std::move(upC));
					success_transitions.setTransitionTo(pC);
					success_transitions.emplace(&pC->success_transition);
					continue;
				}
				else if (*s.it == '^')
				{
					UniquePtr<RegexConstraintTransitionable> upC;
					if (s.flags & Regex::multi_line)
					{
						upC = soup::make_unique<RegexStartConstraint<true>>();
					}
					else
					{
						upC = soup::make_unique<RegexStartConstraint<false>>();
					}
					auto pC = upC.get();
					a.constraints.emplace_back(std::move(upC));
					success_transitions.setTransitionTo(pC);
					success_transitions.emplace(&pC->success_transition);
					continue;
				}
				else if (*s.it == '$')
				{
					UniquePtr<RegexConstraintTransitionable> upC;
					if (s.flags & Regex::multi_line)
					{
						upC = soup::make_unique<RegexEndConstraint<true, false>>();
					}
					else if (s.flags & Regex::dollar_end_only)
					{
						upC = soup::make_unique<RegexEndConstraint<false, true>>();
					}
					else
					{
						upC = soup::make_unique<RegexEndConstraint<false, false>>();
					}
					auto pC = upC.get();
					a.constraints.emplace_back(std::move(upC));
					success_transitions.setTransitionTo(pC);
					success_transitions.emplace(&pC->success_transition);
					continue;
					}
			}
			// TODO: UTF-8 mode ('u'nicode flag):
			// - implicitly capture multi-byte symbols in a non-capturing group to avoid jank with '?'
			// - have '.' accept multi-byte symbols as a single symbol
			auto upC = soup::make_unique<RegexCharConstraint>(*s.it);
			auto pC = upC.get();
			a.constraints.emplace_back(std::move(upC));
			success_transitions.setTransitionTo(pC);
			success_transitions.emplace(&pC->success_transition);
		}
		alternatives.emplace_back(std::move(a));
		success_transitions.discharge(alternatives_transitions);

		if (alternatives.size() > 1)
		{
			// Set up rollback transitions for the first constraint in each alternative to jump to next alternative
			for (size_t i = 0; i + 1 != alternatives.size(); ++i)
			{
				alternatives.at(i).constraints.at(0)->rollback_transition = alternatives.at(i + 1).constraints.at(0)->getTransition();
			}
		}

		// Set up group pointers 
		for (const auto& a : alternatives)
		{
			for (const auto& c : a.constraints)
			{
				c->group = this;
			}
		}

		s.alternatives_transitions = std::move(alternatives_transitions);
	}

	std::string RegexGroup::toString() const noexcept
	{
		std::string str{};
		for (const auto& a : alternatives)
		{
			for (const auto& c : a.constraints)
			{
				str.append(c->toString());
			}
			str.push_back('|');
		}
		if (!str.empty())
		{
			str.pop_back();
		}
		return str;
	}

	uint16_t RegexGroup::getFlags() const
	{
		uint16_t set = 0;
		uint16_t unset = 0;
		getFlags(set, unset);
		SOUP_ASSERT((set & unset) == 0);
		return set;
	}

	void RegexGroup::getFlags(uint16_t& set, uint16_t& unset) const noexcept
	{
		for (const auto& a : alternatives)
		{
			for (const auto& c : a.constraints)
			{
				c->getFlags(set, unset);
			}
		}
	}
}
