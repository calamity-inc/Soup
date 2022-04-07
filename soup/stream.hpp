#pragma once

namespace soup
{
	struct stream
	{
		// OM = One More; my name for the integer encoding scheme where every byte's most significant bit is used to indicate if another byte follows.

		template <typename Int>
		[[nodiscard]] static Int readOmInt(std::istream& s)
		{
			Int ret{};
			while (true)
			{
				auto b = s.get();
				if (b == EOF)
				{
					break;
				}
				ret <<= 7;
				ret |= (b & 0x7F);
				if (!(b & 0x80))
				{
					break;
				}
			}
			return ret;
		}

		template <typename Int>
		static void writeOmInt(std::ostream& s, Int val)
		{
			while (val > 0x7F)
			{
				s << ((unsigned char)val | 0x80);
				val >>= 7;
			}
			s << (unsigned char)val;
		}

		template <typename Int>
		static void writeOmInt(std::string& str, Int val)
		{
			while (val > 0x7F)
			{
				str.push_back((unsigned char)val | 0x80);
				val >>= 7;
			}
			str.push_back((unsigned char)val);
		}
	};
}
