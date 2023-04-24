#include "RegexGroup.hpp"

#include "RegexAnyCharConstraint.hpp"
#include "RegexCharConstraint.hpp"
#include "RegexGreedyOneConstraint.hpp"
#include "RegexGroupConstraint.hpp"

namespace soup
{
	RegexGroup::RegexGroup(const ConstructorState& s)
	{
		RegexAlternative a{};
		const RegexConstraint** pSuccessTransition = &initial;
		bool force_success = false;
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
					continue;
				}
				else if (*s.it == '(')
				{
					++s.it;
					auto upGC = soup::make_unique<RegexGroupConstraint>(s);
					*pSuccessTransition = upGC->group.initial;
					pSuccessTransition = upGC->getSuccessTransitionPointer();
					force_success = false;
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
					*pSuccessTransition = upGreedyConstraint.get();

					// greedy --[success]-> constraint
					upGreedyConstraint->success_transition = pModifiedConstraint->getTransition();

					// greedy --[rollback]-> next-constraint
					pSuccessTransition = &upGreedyConstraint->rollback_transition;

					// If we don't have a next constraint...
					force_success = true;

					a.constraints.back() = std::move(upGreedyConstraint);
					continue;
				}
				else if (*s.it == '.')
				{
					auto c = a.constraints.emplace_back(soup::make_unique<RegexAnyCharConstraint>()).get();
					*pSuccessTransition = c;
					pSuccessTransition = &c->success_transition;
					force_success = false;
					continue;
				}
			}
			// TODO: UTF-8 mode:
			// - implicitly capture multi-byte symbols in a non-capturing group to avoid jank with '?'
			// - have '.' accept multi-byte symbols as a single symbol
			auto c = a.constraints.emplace_back(soup::make_unique<RegexCharConstraint>(*s.it)).get();
			*pSuccessTransition = c;
			pSuccessTransition = &c->success_transition;
			force_success = false;
		}
		alternatives.emplace_back(std::move(a));

		if (force_success)
		{
			*reinterpret_cast<uintptr_t*>(pSuccessTransition) = 1;
		}

		if (alternatives.size() > 1)
		{
			// Copy success transition into the last constraint of all alternatives
			for (auto& a : alternatives)
			{
				a.constraints.back()->success_transition = *pSuccessTransition;
			}

			// Set up rollback transitions for the first constraint in each alternative to jump to next alternative
			for (size_t i = 0; i + 1 != alternatives.size(); ++i)
			{
				alternatives.at(i).constraints.at(0)->rollback_transition = alternatives.at(i + 1).constraints.at(0);
			}
		}
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
