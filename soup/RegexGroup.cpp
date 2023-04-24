#include "RegexGroup.hpp"

#include "RegexAnyCharConstraint.hpp"
#include "RegexCharConstraint.hpp"
#include "RegexGreedyOneConstraint.hpp"
#include "RegexGroupConstraint.hpp"

namespace soup
{
	struct TransitionsVector
	{
		std::vector<const RegexConstraint**> data;

		void emplace(const RegexConstraint** p)
		{
			data.emplace_back(p);
		}

		void setTransitionTo(const RegexConstraint* c) noexcept
		{
			for (const auto& p : data)
			{
				*p = c;
			}
			data.clear();
		}

		void discharge(std::vector<const RegexConstraint**>& outTransitions) noexcept
		{
			for (const auto& p : data)
			{
				outTransitions.emplace_back(p);
			}
			data.clear();
		}
	};

	RegexGroup::RegexGroup(const ConstructorState& s)
	{
		TransitionsVector success_transitions;
		success_transitions.data = { &initial };

		RegexAlternative a{};

		std::vector<const RegexConstraint**> alternatives_transitions{};

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
					++s.it;
					auto upGC = soup::make_unique<RegexGroupConstraint>(s);
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
				else if (*s.it == '.')
				{
					auto c = a.constraints.emplace_back(soup::make_unique<RegexAnyCharConstraint>()).get();
					success_transitions.setTransitionTo(c);
					success_transitions.emplace(&c->success_transition);
					continue;
				}
			}
			// TODO: UTF-8 mode:
			// - implicitly capture multi-byte symbols in a non-capturing group to avoid jank with '?'
			// - have '.' accept multi-byte symbols as a single symbol
			auto c = a.constraints.emplace_back(soup::make_unique<RegexCharConstraint>(*s.it)).get();
			success_transitions.setTransitionTo(c);
			success_transitions.emplace(&c->success_transition);
		}
		alternatives.emplace_back(std::move(a));
		success_transitions.discharge(alternatives_transitions);

		if (alternatives.size() > 1)
		{
			// Set up rollback transitions for the first constraint in each alternative to jump to next alternative
			for (size_t i = 0; i + 1 != alternatives.size(); ++i)
			{
				alternatives.at(i).constraints.at(0)->rollback_transition = alternatives.at(i + 1).constraints.at(0);
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
}
