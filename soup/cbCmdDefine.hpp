#pragma once

#include "cbCmd.hpp"

#include "Dictionary.hpp"
#include "SharedPtr.hpp"

namespace soup
{
	struct cbCmdDefine : public cbCmd
	{
		inline static SharedPtr<Dictionary> dict;
		/*** An example of initialising this:
		* FileReader fr("Soup/resources/dictionary.bin");
		* cbCmdDefine::dict = soup::make_shared<Dictionary>();
		* cbCmdDefine::dict->read(fr);
		*/

		[[nodiscard]] std::vector<std::string> getTriggers() const noexcept final
		{
			return { "define" };
		}

		[[nodiscard]] std::string getResponse(cbParser& p) const noexcept final
		{
			if (!dict)
			{
				return "I'd love to help you with definitions, but soup::cbCmdDefine::dict is uninitialised, so I have no data. :|";
			}
			auto word = p.getArgWord();
			if (!word.empty())
			{
				if (auto dw = dict->find(word);
					dw && !dw->meanings.empty()
					)
				{
					for (const auto& meaning : dw->meanings)
					{
						word.push_back('\n');
						word.append(meaning.meaning);
					}
				}
				else
				{
					word.append(" is not in my dictionary");
				}
				return word;
			}
			return "Define what?";
		}
	};
}
