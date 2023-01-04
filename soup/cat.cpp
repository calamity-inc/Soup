#include "cat.hpp"

#include "Reader.hpp"
#include "string.hpp"

namespace soup
{
	UniquePtr<catNode> catParse(Reader& r)
	{
		std::string spaces;
		auto root = soup::make_unique<catNode>(nullptr);
		std::vector<catNode*> depths{ root.get() };
		size_t depth = 0;
		std::string line;
		while (r.getLine(line)) // Could possibly have some mechanism for escaping NL e.g. "\\\n" to allow for multi-line values
		{
			SOUP_IF_UNLIKELY (line.empty())
			{
				continue;
			}

			if (spaces.empty())
			{
				if (depth == 0
					&& line.at(0) == ' '
					)
				{
					// Since we're just at depth 0, spaces at the beginning of a line will tell us how many spaces are supposed to equal a tab in the user's view.
					// We will then automatically convert that amount of spaces to tabs.
					size_t num_spaces = 0;
					for (const auto& c : line)
					{
						if (c != ' ')
						{
							break;
						}
						++num_spaces;
					}
					spaces = std::string(num_spaces, ' ');
					goto _convert_spaces_to_tabs;
				}
			}
			else
			{
			_convert_spaces_to_tabs:
				size_t tabs = 0;
				while (line.substr(0, spaces.length()) == spaces)
				{
					line.erase(0, spaces.length());
					++tabs;
				}
				line.insert(0, tabs, '\t');
			}

			// Descend
			if (line.length() <= depth
				|| line.substr(0, depth) != std::string(depth, '\t')
				)
			{
				do
				{
					SOUP_ASSERT(depth != 0);
					depths.pop_back();
					--depth;
				} while (line.length() <= depth
					|| line.substr(0, depth) != std::string(depth, '\t')
					);
			}
			else
			{
				// Ascend
				if (line.length() > depth
					&& line.substr(0, depth + 1) == std::string(depth + 1, '\t')
					)
				{
					SOUP_ASSERT(!depths.at(depth)->children.empty());
					depths.emplace_back(depths.at(depth)->children.back());
					++depth;
				}
			}

			// Content
			auto node = depths.at(depth)->children.emplace_back(soup::make_unique<catNode>(depths.at(depth))).get();
			auto line_trimmed = line.substr(depth);
			size_t delim = 0;
			bool with_escaping = false;
			while (true)
			{
				delim = line_trimmed.find(':', delim);
				if (delim == std::string::npos
					|| delim == 0
					|| line_trimmed.at(delim - 1) != '\\'
					)
				{
					break;
				}
				with_escaping = true;
				++delim;
			}
			if (delim != std::string::npos)
			{
				node->name = line_trimmed.substr(0, delim);
				SOUP_ASSERT(line_trimmed.at(delim + 1) == ' ');
				node->value = line_trimmed.substr(delim + 2); // ": "
			}
			else
			{
				node->name = line_trimmed;
			}
			if (with_escaping)
			{
				string::replaceAll(node->name, "\\:", ":");
			}
			SOUP_ASSERT(!node->name.empty());
			SOUP_ASSERT(node->name.at(0) != '\t');
		}
		return root;
	}
}
