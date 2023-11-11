#include "lyoStylesheet.hpp"

#include "joaat.hpp"
#include "LangDesc.hpp"
#include "Lexeme.hpp"

//#include <iostream>

namespace soup
{
	void lyoStylesheet::addRulesFromCss(const std::string& css)
	{
		LangDesc ld;
		ld.semicolon_is_not_space = true;
		auto tks = ld.tokenise(css);
		ld.eraseSpace(tks);

		lyoRule rule;
		std::string property_name;
		for (auto tk = tks.begin(); tk != tks.end(); ++tk)
		{
			if (rule.selector.empty())
			{
				rule.selector = tk->getLiteral();
				SOUP_ASSERT(++tk != tks.end() && tk->isLiteral("{"));
			}
			else if (property_name.empty())
			{
				property_name = tk->getLiteral();
				SOUP_ASSERT(++tk != tks.end() && tk->isLiteral(":"));
			}
			else
			{
				//std::cout << "Setting value of " << property_name << " to " << tk->toString() << "\n";
				switch (joaat::hash(property_name))
				{
				case joaat::hash("display"):
					rule.style.display_block = (tk->getLiteral() == "block");
					break;

				case joaat::hash("margin-left"):
					rule.style.margin_left = static_cast<uint8_t>(tk->val.getInt());
					break;

				case joaat::hash("margin-right"):
					rule.style.margin_right = static_cast<uint8_t>(tk->val.getInt());
					break;

				case joaat::hash("margin-top"):
					rule.style.margin_top = static_cast<uint8_t>(tk->val.getInt());
					break;

				case joaat::hash("margin-bottom"):
					rule.style.margin_bottom = static_cast<uint8_t>(tk->val.getInt());
					break;
				
				case joaat::hash("font-size"):
					rule.style.font_size = static_cast<uint8_t>(tk->val.getInt());
					break;

				case joaat::hash("color"):
				case joaat::hash("colour"): // the world if this were valid CSS: https://i.imgflip.com/2ynjel.jpg
					rule.style.color = Rgb::fromName(tk->getLiteral());
					break;

				case joaat::hash("background-color"):
				case joaat::hash("background-colour"):
					rule.style.background_color = Rgb::fromName(tk->getLiteral());
					break;
				}
				property_name.clear();
				if (++tk != tks.end())
				{
					if (tk->isLiteral(";"))
					{
						if (++tk == tks.end())
						{
							--tk;
							continue;
						}
					}
					if (tk->isLiteral("}"))
					{
						rules.emplace_back(std::move(rule));
						rule.selector.clear();
					}
					else
					{
						--tk;
					}
				}
				else
				{
					--tk;
				}
			}
		}
		if (!rule.selector.empty())
		{
			rules.emplace_back(std::move(rule));
		}
	}
}
