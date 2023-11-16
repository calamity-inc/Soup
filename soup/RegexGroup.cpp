#include "RegexGroup.hpp"

#include "RegexFlags.hpp"
#include "string.hpp"
#include "unicode.hpp"

#include "RegexAnyCharConstraint.hpp"
#include "RegexCharConstraint.hpp"
#include "RegexCodepointConstraint.hpp"
#include "RegexDummyConstraint.hpp"
#include "RegexEndConstraint.hpp"
#include "RegexExactQuantifierConstraint.hpp"
#include "RegexGroupConstraint.hpp"
#include "RegexNegativeLookaheadConstraint.hpp"
#include "RegexNegativeLookbehindConstraint.hpp"
#include "RegexOpenEndedRangeQuantifierConstraint.hpp"
#include "RegexPositiveLookaheadConstraint.hpp"
#include "RegexPositiveLookbehindConstraint.hpp"
#include "RegexOptConstraint.hpp"
#include "RegexQuantifierConstraint.hpp"
#include "RegexRangeQuantifierConstraint.hpp"
#include "RegexRangeConstraint.hpp"
#include "RegexStartConstraint.hpp"
#include "RegexWordBoundaryConstraint.hpp"
#include "RegexWordCharConstraint.hpp"

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

		void emplaceRollback(const RegexConstraintTransitionable** p)
		{
			data.emplace_back(p);

			// If we don't have a next constraint, rollback is match success.
			*reinterpret_cast<uintptr_t*>(p) = 1;
		}

		void setPreviousTransitionTo(const RegexConstraintTransitionable* c) noexcept
		{
			for (const auto& p : prev_data)
			{
				*p = c;
			}
		}

		void setTransitionTo(const RegexConstraintTransitionable* c, bool save_checkpoint = false) noexcept
		{
			SOUP_ASSERT((reinterpret_cast<uintptr_t>(c) & 1) == 0);
			if (save_checkpoint)
			{
				reinterpret_cast<uintptr_t&>(c) |= 1;
			}

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

		void rollback() noexcept
		{
			data = std::move(prev_data);
			prev_data.clear();

			for (const auto& p : data)
			{
				*p = nullptr;
			}
		}
	};

	static void discharge_alternative(RegexGroup& g, TransitionsVector& success_transitions, RegexAlternative& a)
	{
		// Ensure all alternatives have at least one constraint so we can set up transitions
		if (a.constraints.empty())
		{
			auto upC = soup::make_unique<RegexDummyConstraint>();
			auto pC = upC.get();
			a.constraints.emplace_back(std::move(upC));
			success_transitions.setTransitionTo(pC);
			success_transitions.emplace(&pC->success_transition);
		}

		g.alternatives.emplace_back(std::move(a));
		a.constraints.clear();
	}

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
				if (*s.it == 'b')
				{
					auto upC = soup::make_unique<RegexWordBoundaryConstraint<false>>();
					auto pC = upC.get();
					a.constraints.emplace_back(std::move(upC));
					success_transitions.setTransitionTo(pC);
					success_transitions.emplace(&pC->success_transition);
					continue;
				}
				else if (*s.it == 'B')
				{
					auto upC = soup::make_unique<RegexWordBoundaryConstraint<true>>();
					auto pC = upC.get();
					a.constraints.emplace_back(std::move(upC));
					success_transitions.setTransitionTo(pC);
					success_transitions.emplace(&pC->success_transition);
					continue;
				}
				else if (*s.it == 'w')
				{
					auto upC = soup::make_unique<RegexWordCharConstraint<false>>();
					auto pC = upC.get();
					a.constraints.emplace_back(std::move(upC));
					success_transitions.setTransitionTo(pC);
					success_transitions.emplace(&pC->success_transition);
					continue;
				}
				else if (*s.it == 'W')
				{
					auto upC = soup::make_unique<RegexWordCharConstraint<true>>();
					auto pC = upC.get();
					a.constraints.emplace_back(std::move(upC));
					success_transitions.setTransitionTo(pC);
					success_transitions.emplace(&pC->success_transition);
					continue;
				}
				else if (*s.it == 'A')
				{
					auto upC = soup::make_unique<RegexStartConstraint<true, false>>();
					auto pC = upC.get();
					a.constraints.emplace_back(std::move(upC));
					success_transitions.setTransitionTo(pC);
					success_transitions.emplace(&pC->success_transition);
					continue;
				}
				else if (*s.it == 'Z')
				{
					auto upC = soup::make_unique<RegexEndConstraint<true, false, false>>();
					auto pC = upC.get();
					a.constraints.emplace_back(std::move(upC));
					success_transitions.setTransitionTo(pC);
					success_transitions.emplace(&pC->success_transition);
					continue;
				}
				else if (*s.it == 'z')
				{
					auto upC = soup::make_unique<RegexEndConstraint<true, false, true>>();
					auto pC = upC.get();
					a.constraints.emplace_back(std::move(upC));
					success_transitions.setTransitionTo(pC);
					success_transitions.emplace(&pC->success_transition);
					continue;
				}
				else if (*s.it == 'd')
				{
					auto upC = soup::make_unique<RegexRangeConstraint>(RegexRangeConstraint::digits);
					auto pC = upC.get();
					a.constraints.emplace_back(std::move(upC));
					success_transitions.setTransitionTo(pC);
					success_transitions.emplace(&pC->success_transition);
					continue;
				}
				else if (*s.it == 's')
				{
					auto upC = soup::make_unique<RegexRangeConstraint>(RegexRangeConstraint::whitespace);
					auto pC = upC.get();
					a.constraints.emplace_back(std::move(upC));
					success_transitions.setTransitionTo(pC);
					success_transitions.emplace(&pC->success_transition);
					continue;
				}
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
					discharge_alternative(*this, success_transitions, a);
					success_transitions.discharge(alternatives_transitions);
					continue;
				}
				else if (*s.it == '(')
				{
					bool non_capturing = false;
					bool positive_lookahead = false;
					bool negative_lookahead = false;
					bool positive_lookbehind = false;
					bool negative_lookbehind = false;
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
							else if (*s.it == '=')
							{
								positive_lookahead = true;
								++s.it;
							}
							else if (*s.it == '!')
							{
								negative_lookahead = true;
								++s.it;
							}
							else if (*s.it == '<')
							{
								if (++s.it != s.end)
								{
									if (*s.it == '=')
									{
										positive_lookbehind = true;
										++s.it;
									}
									else if (*s.it == '!')
									{
										negative_lookbehind = true;
										++s.it;
									}
								}
							}
						}
					}
					if (positive_lookahead)
					{
						auto upGC = soup::make_unique<RegexPositiveLookaheadConstraint>(s);
						upGC->group.parent = this;
						upGC->group.lookahead_or_lookbehind = true;

						if (upGC->group.initial)
						{
							// last-constraint --[success]-> first-lookahead-constraint + save checkpoint
							success_transitions.setTransitionTo(upGC->group.initial, true);
							success_transitions.data = std::move(s.alternatives_transitions);

							// last-lookahead-constraint --[success]-> group (to restore checkpoint)
							success_transitions.setTransitionTo(upGC.get());

							// group --> next-constraint
							success_transitions.emplace(&upGC->success_transition);
						}

						a.constraints.emplace_back(std::move(upGC));
					}
					else if (negative_lookahead)
					{
						auto upGC = soup::make_unique<RegexNegativeLookaheadConstraint>(s);
						upGC->group.parent = this;
						upGC->group.lookahead_or_lookbehind = true;

						if (upGC->group.initial)
						{
							// last-constraint --[success]-> first-lookahead-constraint
							success_transitions.setTransitionTo(upGC->group.initial);
							success_transitions.data = std::move(s.alternatives_transitions);
						}

						// last-lookahead-constraint --[success]-> fail
						success_transitions.setTransitionTo(reinterpret_cast<const RegexConstraintTransitionable*>(0b10));

						if (upGC->group.initial)
						{
							// first-lookahead-constraint --[rollback]-> next-constraint
							success_transitions.emplaceRollback(&const_cast<RegexConstraintTransitionable*>(upGC->group.initial)->rollback_transition);
						}

						a.constraints.emplace_back(std::move(upGC));
					}
					else if (positive_lookbehind)
					{
						UniquePtr<RegexConstraintLookbehind> upGC;
						if (s.hasFlag(RE_UNICODE))
						{
							upGC = soup::make_unique<RegexPositiveLookbehindConstraint<true>>(s);
						}
						else
						{
							upGC = soup::make_unique<RegexPositiveLookbehindConstraint<false>>(s);
						}
						upGC->group.parent = this;
						upGC->group.lookahead_or_lookbehind = true;
						upGC->window = upGC->group.getCursorAdvancement();

						// last-constraint --[success]-> group (to move cursor)
						success_transitions.setTransitionTo(upGC.get());

						// group --> first-lookbehind-constraint
						success_transitions.emplace(&upGC->success_transition);
						success_transitions.setTransitionTo(upGC->group.initial);

						// last-lookbehind-constraint --[success]-> next-constraint
						success_transitions.data = std::move(s.alternatives_transitions);

						a.constraints.emplace_back(std::move(upGC));
					}
					else if (negative_lookbehind)
					{
						UniquePtr<RegexConstraintLookbehind> upGC;
						if (s.hasFlag(RE_UNICODE))
						{
							upGC = soup::make_unique<RegexNegativeLookbehindConstraint<true>>(s);
						}
						else
						{
							upGC = soup::make_unique<RegexNegativeLookbehindConstraint<false>>(s);
						}
						upGC->group.parent = this;
						upGC->group.lookahead_or_lookbehind = true;
						upGC->window = upGC->group.getCursorAdvancement();

						// last-constraint --[success]-> group (to move cursor)
						success_transitions.setTransitionTo(upGC.get());

						// group --> first-lookbehind-constraint
						success_transitions.emplace(&upGC->success_transition);
						success_transitions.setTransitionTo(upGC->group.initial);

						// last-lookbehind-constraint --[success]-> fail
						success_transitions.data = std::move(s.alternatives_transitions);
						success_transitions.setTransitionTo(reinterpret_cast<const RegexConstraintTransitionable*>(0b10));

						// group --[rollback]--> next-constraint
						success_transitions.emplaceRollback(&upGC->rollback_transition);

						a.constraints.emplace_back(std::move(upGC));
					}
					else
					{
						if (s.hasFlag(RE_EXPLICIT_CAPTURE) && name.empty())
						{
							non_capturing = true;
						}
						auto upGC = soup::make_unique<RegexGroupConstraint>(s, non_capturing);
						upGC->group.parent = this;
						upGC->group.name = std::move(name);
						success_transitions.setTransitionTo(upGC->group.initial);
						success_transitions.data = std::move(s.alternatives_transitions);
						a.constraints.emplace_back(std::move(upGC));
					}
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
					bool greedy = true;
					if (s.it + 1 != s.end
						&& *(s.it + 1) == '?'
						)
					{
						greedy = false;
						++s.it;
					}
					greedy ^= s.hasFlag(RE_UNGREEDY);

					RegexConstraint* pModifiedConstraint;
					UniquePtr<RegexConstraintTransitionable> upQuantifierConstraint;
					if (greedy)
					{
						UniquePtr<RegexConstraint> upModifiedConstraint = std::move(a.constraints.back());
						pModifiedConstraint = upModifiedConstraint.get();
						upQuantifierConstraint = soup::make_unique<RegexQuantifierConstraint<true, true>>(std::move(upModifiedConstraint));
					}
					else
					{
						UniquePtr<RegexConstraint> upModifiedConstraint = std::move(a.constraints.back());
						pModifiedConstraint = upModifiedConstraint.get();
						upQuantifierConstraint = soup::make_unique<RegexQuantifierConstraint<true, false>>(std::move(upModifiedConstraint));
					}

					pModifiedConstraint->group = this;

					// constraint --[success]-> quantifier
					success_transitions.setTransitionTo(upQuantifierConstraint.get());

					if (greedy)
					{
						// quantifier --[success]-> constraint
						upQuantifierConstraint->success_transition = pModifiedConstraint->getTransition();

						// quantifier --[rollback]-> next-constraint
						success_transitions.emplaceRollback(&upQuantifierConstraint->rollback_transition);
					}
					else
					{
						// quantifier --[success]-> next-constraint
						success_transitions.emplace(&upQuantifierConstraint->success_transition);

						// quantifier --[rollback]-> constraint
						upQuantifierConstraint->rollback_transition = pModifiedConstraint->getTransition();
					}

					a.constraints.back() = std::move(upQuantifierConstraint);
					continue;
				}
				else if (*s.it == '*')
				{
					bool greedy = true;
					if (s.it + 1 != s.end
						&& *(s.it + 1) == '?'
						)
					{
						greedy = false;
						++s.it;
					}
					greedy ^= s.hasFlag(RE_UNGREEDY);

					RegexConstraint* pModifiedConstraint;
					UniquePtr<RegexConstraintTransitionable> upQuantifierConstraint;
					if (greedy)
					{
						UniquePtr<RegexConstraint> upModifiedConstraint = std::move(a.constraints.back());
						pModifiedConstraint = upModifiedConstraint.get();
						upQuantifierConstraint = soup::make_unique<RegexQuantifierConstraint<false, true>>(std::move(upModifiedConstraint));
					}
					else
					{
						UniquePtr<RegexConstraint> upModifiedConstraint = std::move(a.constraints.back());
						pModifiedConstraint = upModifiedConstraint.get();
						upQuantifierConstraint = soup::make_unique<RegexQuantifierConstraint<false, false>>(std::move(upModifiedConstraint));
					}

					pModifiedConstraint->group = this;

					// prev-constraint --[success]-> quantifier
					success_transitions.setPreviousTransitionTo(upQuantifierConstraint.get());

					// constraint --[success]-> quantifier
					success_transitions.setTransitionTo(upQuantifierConstraint.get());

					if (greedy)
					{
						// quantifier --[success]-> constraint
						upQuantifierConstraint->success_transition = pModifiedConstraint->getTransition();

						// quantifier --[rollback]-> next-constraint
						success_transitions.emplaceRollback(&upQuantifierConstraint->rollback_transition);
					}
					else
					{
						// quantifier --[success]-> next-constraint
						success_transitions.emplace(&upQuantifierConstraint->success_transition);

						// quantifier --[rollback]-> constraint
						upQuantifierConstraint->rollback_transition = pModifiedConstraint->getTransition();
					}

					a.constraints.back() = std::move(upQuantifierConstraint);
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

					// constraint --[success]-> next-constraint
					// opt --[rollback]-> next-constraint
					success_transitions.emplaceRollback(&upOptConstraint->rollback_transition);

					a.constraints.back() = std::move(upOptConstraint);
					continue;
				}
				else if (*s.it == '.')
				{
					UniquePtr<RegexConstraintTransitionable> upC;
					if (s.hasFlag(RE_DOTALL))
					{
						if (s.hasFlag(RE_UNICODE))
						{
							upC = soup::make_unique<RegexAnyCharConstraint<true, true>>();
						}
						else
						{
							upC = soup::make_unique<RegexAnyCharConstraint<true, false>>();
						}
					}
					else
					{
						if (s.hasFlag(RE_UNICODE))
						{
							upC = soup::make_unique<RegexAnyCharConstraint<false, true>>();
						}
						else
						{
							upC = soup::make_unique<RegexAnyCharConstraint<false, false>>();
						}
					}
					auto pC = upC.get();
					a.constraints.emplace_back(std::move(upC));
					success_transitions.setTransitionTo(pC);
					success_transitions.emplace(&pC->success_transition);
					continue;
				}
				else if (*s.it == '[')
				{
					auto upC = soup::make_unique<RegexRangeConstraint>(s.it, s.end, s.hasFlag(RE_INSENSITIVE));
					auto pC = upC.get();
					a.constraints.emplace_back(std::move(upC));
					success_transitions.setTransitionTo(pC);
					success_transitions.emplace(&pC->success_transition);
					if (s.it == s.end)
					{
						break;
					}
					continue;
				}
				else if (*s.it == '^')
				{
					UniquePtr<RegexConstraintTransitionable> upC;
					if (s.flags & RE_MULTILINE)
					{
						upC = soup::make_unique<RegexStartConstraint<false, true>>();
					}
					else
					{
						upC = soup::make_unique<RegexStartConstraint<false, false>>();
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
					if (s.flags & RE_MULTILINE)
					{
						upC = soup::make_unique<RegexEndConstraint<false, true, false>>();
					}
					else if (s.flags & RE_DOLLAR_ENDONLY)
					{
						upC = soup::make_unique<RegexEndConstraint<false, false, true>>();
					}
					else
					{
						upC = soup::make_unique<RegexEndConstraint<false, false, false>>();
					}
					auto pC = upC.get();
					a.constraints.emplace_back(std::move(upC));
					success_transitions.setTransitionTo(pC);
					success_transitions.emplace(&pC->success_transition);
					continue;
				}
				else if (*s.it == '{')
				{
					size_t min_reps = 0;
					while (++s.it != s.end && string::isNumberChar(*s.it))
					{
						min_reps *= 10;
						min_reps += ((*s.it) - '0');
					}
					if (s.it == s.end)
					{
						break;
					}

					bool exact = true;
					size_t max_reps = 0;
					if (*s.it == ',')
					{
						exact = false;
						while (++s.it != s.end && string::isNumberChar(*s.it))
						{
							max_reps *= 10;
							max_reps += ((*s.it) - '0');
						}
						if (s.it == s.end)
						{
							break;
						}
					}

					UniquePtr<RegexConstraint> upModifiedConstraint = std::move(a.constraints.back());
					auto pModifiedConstraint = upModifiedConstraint.get();
					if (min_reps == 0)
					{
						success_transitions.rollback();
						a.constraints.pop_back();
					}
					else if (exact || min_reps == max_reps) // {X} or {X,X}
					{
						auto upRepConstraint = soup::make_unique<RegexExactQuantifierConstraint>();
						upRepConstraint->constraints.emplace_back(std::move(upModifiedConstraint));
						pModifiedConstraint->group = this;
						while (--min_reps != 0)
						{
							auto upClone = pModifiedConstraint->clone();
							upClone->group = this;

							// constraint --[success]-> clone
							success_transitions.setTransitionTo(upClone->getTransition());

							// clone --[success]-> whatever-comes-next
							success_transitions.emplace(&upClone->success_transition);

							upRepConstraint->constraints.emplace_back(std::move(upClone));
						}
						a.constraints.back() = std::move(upRepConstraint);
					}
					else if (max_reps == 0) // {X,}
					{
						auto upRepConstraint = soup::make_unique<RegexOpenEndedRangeQuantifierConstraint>();
						upRepConstraint->constraints.emplace_back(std::move(upModifiedConstraint));
						pModifiedConstraint->group = this;
						while (--min_reps != 0)
						{
							auto upClone = pModifiedConstraint->clone();
							upClone->group = this;

							// constraint --[success]-> clone
							success_transitions.setTransitionTo(upClone->getTransition());

							// clone --[success]-> whatever-comes-next
							success_transitions.emplace(&upClone->success_transition);

							upRepConstraint->constraints.emplace_back(std::move(upClone));
						}

						// last-clone --[success]-> quantifier
						success_transitions.setTransitionTo(upRepConstraint.get());

						// quantifier --[success]-> last-clone
						upRepConstraint->success_transition = upRepConstraint->constraints.back()->getTransition();

						// quantifier --[rollback]-> next-constraint
						success_transitions.emplaceRollback(&upRepConstraint->rollback_transition);

						a.constraints.back() = std::move(upRepConstraint);
					}
					else if (min_reps < max_reps) // {X,Y}
					{
						auto upRepConstraint = soup::make_unique<RegexRangeQuantifierConstraint>();
						upRepConstraint->constraints.emplace_back(std::move(upModifiedConstraint));
						upRepConstraint->min_reps = min_reps;
						pModifiedConstraint->group = this;
						size_t required_reps = min_reps;
						while (--required_reps != 0)
						{
							auto upClone = pModifiedConstraint->clone();
							upClone->group = this;

							// constraint --[success]-> clone
							success_transitions.setTransitionTo(upClone->getTransition());

							// clone --[success]-> whatever-comes-next
							success_transitions.emplace(&upClone->success_transition);

							upRepConstraint->constraints.emplace_back(std::move(upClone));
						}
						TransitionsVector rep_transitions;
						success_transitions.discharge(rep_transitions.data);
						for (size_t optional_reps = (max_reps - min_reps); optional_reps != 0; --optional_reps)
						{
							auto upClone = pModifiedConstraint->clone();
							upClone->group = this;

							// constraint --[success]-> clone
							rep_transitions.setTransitionTo(upClone->getTransition());

							// clone --[success]-> next-clone
							rep_transitions.emplace(&upClone->success_transition);

							// clone --[rollback]-> next-constraint
							success_transitions.emplaceRollback(&upClone->rollback_transition);

							upRepConstraint->constraints.emplace_back(std::move(upClone));
						}

						// last-clone --[success]-> next-constraint
						rep_transitions.discharge(success_transitions.data);

						a.constraints.back() = std::move(upRepConstraint);
					}
					else
					{
						// We may be here if (!exact && min_reps > max_reps)
						// Which is invalid, so we just yeet the constraint as if {0} was written.
						success_transitions.rollback();
						a.constraints.pop_back();
					}
					continue;
				}
			}

			UniquePtr<RegexConstraintTransitionable> upC;
			if (UTF8_HAS_CONTINUATION(*s.it) && s.hasFlag(RE_UNICODE))
			{
				std::string c;
				do
				{
					c.push_back(*s.it);
				} while (s.it + 1 != s.end && UTF8_IS_CONTINUATION(*++s.it));
				upC = soup::make_unique<RegexCodepointConstraint>(std::move(c));
			}
			else if (s.hasFlag(RE_INSENSITIVE) && string::lower_char(*s.it) != string::upper_char(*s.it))
			{
				const char arr[] = { string::lower_char(*s.it), string::upper_char(*s.it) };
				upC = soup::make_unique<RegexRangeConstraint>(arr);
			}
			else
			{
				upC = soup::make_unique<RegexCharConstraint>(*s.it);
			}
			auto pC = upC.get();
			a.constraints.emplace_back(std::move(upC));
			success_transitions.setTransitionTo(pC);
			success_transitions.emplace(&pC->success_transition);
		}
		discharge_alternative(*this, success_transitions, a);
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
		SOUP_ASSERT((set & unset) == 0, "RegexGroup has contradicting flags");
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

	size_t RegexGroup::getCursorAdvancement() const
	{
		size_t accum = 0;
		for (const auto& a : alternatives)
		{
			for (const auto& c : a.constraints)
			{
				accum += c->getCursorAdvancement();
			}
		}
		return accum;
	}
}
