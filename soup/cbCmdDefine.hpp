#pragma once

#include "cbCmd.hpp"

#include "Dictionary.hpp"
#include "SharedPtr.hpp"

NAMESPACE_SOUP
{
	struct cbCmdDefine : public cbCmd
	{
		inline static SharedPtr<Dictionary> dict;
		/*** To initialise this, use Chatbot::intialiseResources or:
		* FileReader fr("Soup/resources/dictionary.bin");
		* cbCmdDefine::dict = soup::make_shared<Dictionary>();
		* cbCmdDefine::dict->read(fr);
		*/

		[[nodiscard]] RegexMatchResult checkTriggers(const std::string& str) const final
		{
			static Regex r(R"((?:define|definition of)\s+(?'word'\w+))", RE_INSENSITIVE);
			return r.search(str);
		}

		[[nodiscard]] cbResult process(const RegexMatchResult& m) const final
		{
			if (!dict)
			{
				return "I'd love to help you with definitions, but soup::cbCmdDefine::dict is uninitialised, so I have no data. :|";
			}
			auto word = m.findGroupByName("word")->toString();
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
				return cbResult(std::move(word));
			}
			return "Define what?";
		}
	};
}
