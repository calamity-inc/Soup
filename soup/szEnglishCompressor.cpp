#include "szEnglishCompressor.hpp"

#include "BitReader.hpp"
#include "bitutil.hpp"
#include "Dictionary.hpp"
#include "string.hpp"
#include "StringBuilder.hpp"
#include "WebResource.hpp"

#define DEBUG_ENG_DEC false

#if DEBUG_ENG_DEC
#include <iostream>
#endif

namespace soup
{
	static std::vector<std::string> getWordListImpl()
	{
		std::vector<std::string> english_words{};

		{
			WebResource wr("raw.githubusercontent.com", "/calamity-inc/Soup/senpai/resources/dictionary.bin");
			wr.downloadWithStaleCaching();
			StringReader sr(std::move(wr.data));
			Dictionary dict;
			dict.read(sr);
			for (auto& word : dict.words)
			{
				english_words.emplace_back(std::move(word.first));
			}
		}

		english_words.emplace_back("of");
		english_words.emplace_back("to");
		english_words.emplace_back("and");
		english_words.emplace_back("for");
		english_words.emplace_back("the");
		english_words.emplace_back("onto");
		english_words.emplace_back("than");
		english_words.emplace_back("that");
		english_words.emplace_back("with");
		english_words.emplace_back("should");

		english_words.emplace_back("encoder");

		std::sort(english_words.begin(), english_words.end(), [](const std::string& a, const std::string& b)
		{
			return a.size() < b.size();
		});

		const auto bits_per_word = bitutil::getBitsNeededToEncodeRange(english_words.size());
		SOUP_ASSERT(bits_per_word <= 17);

		return english_words;
	}

	const std::vector<std::string>& szEnglishCompressor::getWordList()
	{
		static const std::vector<std::string> english_words = getWordListImpl();
		return english_words;
	}

	enum ControlBits : uint8_t
	{
		DICTIONARY_WORD_LOWER = 0,
		DICTIONARY_WORD_TITLE,
		LITERAL_WORD,
		END_OF_STRING,

		NUM_CTRL_BITS = 2
	};

	enum PunctuationBits : uint8_t
	{
		PUNCT_SPACE = 0,
		PUNCT_COMMA_SPACE,
		PUNCT_DOT_SPACE,
		PUNCT_LITERAL,

		NUM_PUNCT_BITS = 2
	};

	struct DictionaryEncoding
	{
		ControlBits control_bits = LITERAL_WORD;
		size_t dictionary_index;

		[[nodiscard]] static DictionaryEncoding get(const std::vector<std::string>& english_words, const std::string& word)
		{
			DictionaryEncoding res;
			if (auto e = std::find(english_words.begin(), english_words.end(), word); e != english_words.end())
			{
				res.control_bits = DICTIONARY_WORD_LOWER;
				res.dictionary_index = (e - english_words.begin());
				//std::cout << "in dictionary: \"" << word << "\" (" << res.dictionary_index << ")\n";
			}
			else
			{
				std::string title_cased_word = word;
				string::lower(title_cased_word);
				title_cased_word[0] = std::toupper(title_cased_word[0]);

				const bool word_is_title_cased = (word == title_cased_word);
				title_cased_word[0] = std::tolower(title_cased_word[0]);

				//std::vector<std::string>::iterator e;
				if (word_is_title_cased)
				{
					e = std::find(english_words.begin(), english_words.end(), title_cased_word);
				}

				if (word_is_title_cased && e != english_words.end())
				{
					res.control_bits = DICTIONARY_WORD_TITLE;
					res.dictionary_index = (e - english_words.begin());
					//std::cout << "in dictionary: \"" << word << "\" (" << res.dictionary_index << ")\n";
				}
				else
				{
					//std::cout << "not in dictionary: \"" << word << "\"\n";
				}
			}
			return res;
		}

		[[nodiscard]] bool isValid() const noexcept
		{
			return control_bits != LITERAL_WORD;
		}

		void commit(BitWriter& bw) const
		{
			bw.u8(NUM_CTRL_BITS, control_bits);
			bw.u17_dyn_2(dictionary_index);
		}
	};

	static void exitpunctmode(BitWriter& bw)
	{
		bw.u8(NUM_PUNCT_BITS, PUNCT_LITERAL);
		bw.str_utf32_lp(std::u32string{}, 2);
	}

	void szEnglishCompressor::compress(BitWriter& bw, const std::string& data) const
	{
		const auto english_words = getWordList();
		bool punctmode = false;
		bool had_letter = false;
		StringBuilder word;
		auto i = data.begin();
		word.beginCopy(data, i);
		for (; i != data.end(); ++i)
		{
			if (string::isLetter(*i) != had_letter)
			{
				had_letter = !had_letter;
				word.endCopy(data, i);
				if (!word.empty())
				{
					if (punctmode)
					{
						if (word == " ")
						{
							//std::cout << "PUNCT_SPACE\n";
							bw.u8(NUM_PUNCT_BITS, PUNCT_SPACE);
						}
						else if (word == ", ")
						{
							//std::cout << "PUNCT_COMMA_SPACE\n";
							bw.u8(NUM_PUNCT_BITS, PUNCT_COMMA_SPACE);
						}
						else if (word == ". ")
						{
							//std::cout << "PUNCT_DOT_SPACE\n";
							bw.u8(NUM_PUNCT_BITS, PUNCT_DOT_SPACE);
						}
						else
						{
							//std::cout << "not a specialised punctuation: \"" << word << "\" (" << word.size() << ")\n";
							auto dictenc = DictionaryEncoding::get(english_words, word);
							if (dictenc.isValid())
							{
								//std::cout << "but " << word << " is in dictionary, so switching modes\n";
								exitpunctmode(bw);
								dictenc.commit(bw);
								punctmode = false;
							}
							else
							{
								bw.u8(NUM_PUNCT_BITS, PUNCT_LITERAL);
								bw.str_utf8_lp(word, 2);
							}
						}
					}
					else
					{
						auto dictenc = DictionaryEncoding::get(english_words, word);
						if (dictenc.isValid())
						{
							dictenc.commit(bw);
						}
						else
						{
							bw.u8(NUM_CTRL_BITS, LITERAL_WORD);
							bw.str_utf8_nt(word);
						}
					}
					punctmode = !punctmode;
				}
				word.clear();
				word.beginCopy(data, i);
			}
		}
		word.endCopy(data, i);
		if (!word.empty())
		{
			//std::cout << "final word: \"" << word << "\"\n";
			if (punctmode)
			{
				bw.u8(NUM_PUNCT_BITS, PUNCT_LITERAL);
				bw.str_utf8_lp(word, 2);
			}
			else
			{
				bw.u8(NUM_CTRL_BITS, LITERAL_WORD);
				bw.str_utf8_nt(word);
			}
			punctmode = !punctmode;
		}
		if (punctmode)
		{
			exitpunctmode(bw);
			punctmode = false;
		}
		bw.u8(NUM_CTRL_BITS, END_OF_STRING);
	}

	std::string szEnglishCompressor::decompress(BitReader& br) const
	{
		std::string res;

		const auto english_words = getWordList();
		bool punctmode = false;
		while (br.hasMore())
		{
			uint8_t op;
			if (punctmode)
			{
				br.u8(NUM_PUNCT_BITS, op);
#if DEBUG_ENG_DEC
				std::cout << "punctmode op " << (int)op;
#endif
				switch (op)
				{
				case PUNCT_SPACE:
					res.push_back(' ');
					break;

				case PUNCT_COMMA_SPACE:
					res.append(", ");
					break;

				case PUNCT_DOT_SPACE:
					res.append(". ");
					break;

				case PUNCT_LITERAL:
					{
						std::string str;
						br.str_utf8_lp(str, 2);
						res.append(str);
#if DEBUG_ENG_DEC
						std::cout << ": " << str;
#endif
					}
					break;
				}
#if DEBUG_ENG_DEC
				std::cout << "\n";
#endif
			}
			else
			{
				br.u8(NUM_CTRL_BITS, op);
#if DEBUG_ENG_DEC
				std::cout << "ctrlmode op " << (int)op << ": ";
#endif
				switch (op)
				{
				case DICTIONARY_WORD_LOWER:
					{
						uint32_t idx;
						br.u17_dyn_2(idx);
#if DEBUG_ENG_DEC
						std::cout << idx << ", " << english_words.at(idx) << "\n";
#endif
						res.append(english_words.at(idx));
					}
					break;

				case DICTIONARY_WORD_TITLE:
					{
						uint32_t idx;
						br.u17_dyn_2(idx);
						std::string title_cased_word = english_words.at(idx);
						title_cased_word[0] = std::toupper(title_cased_word[0]);
#if DEBUG_ENG_DEC
						std::cout << idx << ", " << title_cased_word << "\n";
#endif
						res.append(title_cased_word);
					}
					break;

				case LITERAL_WORD:
					{
						std::string str;
						br.str_utf8_nt(str);
#if DEBUG_ENG_DEC
						std::cout << str << "\n";
#endif
						res.append(str);
					}
					break;

				case END_OF_STRING:
#if DEBUG_ENG_DEC
					std::cout << "end of string\n";
#endif
					goto _break_2;
				}
			}
			punctmode = !punctmode;
		}
	_break_2:;

		return res;
	}
}
