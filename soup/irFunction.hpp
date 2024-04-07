#pragma once

#include <string>
#include <vector>

#include "irType.hpp"
#include "irExpression.hpp"

NAMESPACE_SOUP
{
	struct irFunction
	{
		std::string name;
		std::vector<irType> parameters{};
		std::vector<irType> returns{};
		std::vector<irType> locals{}; // Note: parameters are also locals. This vector begins at parameters.size().
		std::vector<UniquePtr<irExpression>> insns{};

		[[nodiscard]] irType& getLocalType(uint32_t index)
		{
			if (index < parameters.size())
			{
				return parameters.at(index);
			}
			return locals.at(index - parameters.size());
		}

		[[nodiscard]] const irType& getLocalType(uint32_t index) const
		{
			if (index < parameters.size())
			{
				return parameters.at(index);
			}
			return locals.at(index - parameters.size());
		}

		bool optimiseByConstantFolding()
		{
			bool any_changes = false;
			for (auto& insn : insns)
			{
				any_changes |= insn->optimiseByConstantFolding();
			}
			return any_changes;
		}
	};
}
