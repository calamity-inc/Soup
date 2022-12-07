#include "morse.hpp"

#include "aud_math.hpp"
#include "string.hpp"
#include "unicode.hpp"

namespace soup
{
	std::string MorseSequence::toPattern() const
	{
		MorseTiming timing;
		return toPattern(timing);
	}
	
	std::string MorseSequence::toPattern(const MorseTiming& timing) const
	{
		std::string str;
		bool on = true;
		for (const auto& dur : alterndur)
		{
			if (on)
			{
				str.push_back(timing.isDah(dur) ? '-' : '.');
			}
			else
			{
				if (timing.isWordSpace(dur))
				{
					str.append(" / ");
				}
				else if (timing.isLetterSpace(dur))
				{
					str.push_back(' ');
				}
			}
			on = !on;
		}
		return str;
	}

	bool MorseSequence::isOnAt(std::time_t t) const noexcept
	{
		bool on = true;
		std::time_t a = 0;
		for (const auto& dur : alterndur)
		{
			a += dur;
			if (a > t)
			{
				break;
			}
			on = !on;
		}
		return on;
	}

	std::time_t MorseSequence::getDuration() const noexcept
	{
		std::time_t a = 0;
		for (const auto& dur : alterndur)
		{
			a += dur;
		}
		return a;
	}

	double MorseSequence::getAmplitude(double t) const
	{
		if (isOnAt(t * 1000.0))
		{
			return 0.5 * sin(HZ_TO_ANGVEL(550.0) * t);
		}
		return 0.0;
	}

	double MorseSequence::getDurationSeconds() const noexcept
	{
		return (double)getDuration() / 1000.0;
	}

	static std::pair<char16_t, const char*> ALPHA[] = {
		{'A', ".-"},
		{'B', "-..."},
		{'C', "-.-."},
		{'D', "-.."},
		{'E', "."},
		{'F', "..-."},
		{'G', "--."},
		{'H', "...."},
		{'I', ".."},
		{'J', ".---"},
		{'K', "-.-"},
		{'L', ".-.."},
		{'M', "--"},
		{'N', "-."},
		{'O', "---"},
		{'P', ".--."},
		{'Q', "--.-"},
		{'R', ".-."},
		{'S', "..."},
		{'T', "-"},
		{'U', "..-"},
		{'V', "...-"},
		{'W', ".--"},
		{'X', "-..-"},
		{'Y', "-.--"},
		{'Z', "--.."},
		{'1', ".----"},
		{'2', "..---"},
		{'3', "...--"},
		{'4', "....-"},
		{'5', "....."},
		{'6', "-...."},
		{'7', "--..."},
		{'8', "---.."},
		{'9', "----."},
		{'0', "-----"},
		{'+', ".-.-."},
		{'=', "-...-"},
		{'/', "-..-."},
		{'&', ".-..."},
		{'?', "..--.."},
		{'-', "..--.-"},
		{'"', ".-..-."},
		{'.', ".-.-.-"},
		{'@', ".--.-."},
		{'\'', ".----."},
		{'-', "-....-"},
		{';', "-.-.-."},
		{'!', "-.-.--"},
		{'(', "-.--."},
		{')', "-.--.-"},
		{',', "--..--"},
		{':', "---..."},
		{'$', "...-..-"},
		{/* Ä */ 0xE4, ".-.-"},
		{/* Ö */ 0xD6, "---."},
		{/* Ü */ 0xDC, "..--"},
	};

	const char* Morse::encodeLetter(char16_t c)
	{
		if (c >= 'a' && c <= 'z')
		{
			c -= 32;
		}
		for (const auto& e : ALPHA)
		{
			if (e.first == c)
			{
				return e.second;
			}
		}
		return nullptr;
	}

	std::string Morse::encodeWord(const std::string& word)
	{
		std::string str;
		for (const auto& c : word)
		{
			if (!str.empty())
			{
				str.push_back(' ');
			}
			str.append(encodeLetter(c));
		}
		return str;
	}

	std::string Morse::encode(const std::string& text)
	{
		std::string str;
		for (const auto& word : string::explode<std::string>(text, ' '))
		{
			if (!str.empty())
			{
				str.append(" / ");
			}
			str.append(encodeWord(word));
		}
		return str;
	}

	std::string Morse::decode(const std::string& pattern)
	{
		std::string str;
		for (const auto& word_pattern : string::explode<std::string>(pattern, " / "))
		{
			if (!str.empty())
			{
				str.push_back(' ');
			}
			for (const auto& letter_pattern : string::explode<std::string>(word_pattern, ' '))
			{
				for (const auto& e : ALPHA)
				{
					if (e.second == letter_pattern)
					{
						str.append(unicode::utf32_to_utf8(e.first));
						goto _continue_2;
					}
				}
				str.append(unicode::utf32_to_utf8(unicode::REPLACEMENT_CHAR));
			_continue_2:;
			}
		}
		return str;
	}

	MorseSequence Morse::patternToSequence(const std::string& pattern)
	{
		MorseTiming timing;
		return patternToSequence(pattern, timing);
	}

	MorseSequence Morse::patternToSequence(const std::string& pattern, const MorseTiming& timing)
	{
		MorseSequence ms;
		for (auto i = pattern.begin(); i != pattern.end(); ++i)
		{
			if (*i == '.')
			{
				ms.alterndur.emplace_back(timing.getDitLength());
				ms.alterndur.emplace_back(timing.getSpaceLength());
			}
			else if (*i == '-' ||*i == '_')
			{
				ms.alterndur.emplace_back(timing.getDahLength());
				ms.alterndur.emplace_back(timing.getSpaceLength());
			}
			else if (*i == ' ')
			{
				if ((i + 1) != pattern.end()
					&& *(i + 1) == '/'
					&& (i + 2) != pattern.end()
					&& *(i + 2) == ' '
					)
				{
					ms.alterndur.back() = timing.getWordSpaceLength();
					i += 2;
				}
				else
				{
					ms.alterndur.back() = timing.getLetterSpaceLength();
				}
			}
		}
		ms.alterndur.back() = timing.getWordSpaceLength();
		return ms;
	}

	double MorseKey::getAmplitude(double t) const
	{
		if (isDown())
		{
			return 0.5 * sin(HZ_TO_ANGVEL(550.0) * t);
		}
		return 0.0;
	}
}
