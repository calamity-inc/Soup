#include "punycode.hpp"

#include <algorithm> // clamp

#include "base.hpp"

namespace soup
{
	struct PunycodeState
	{
		enum
		{
			base = 36,
			tmin = 1,
			tmax = 26,
			skew = 38,
			damp = 700,
			initial_bias = 72,
			initial_n = 128,
		};

		uint32_t bias = initial_bias;

		[[nodiscard]] constexpr uint32_t t(uint32_t j) noexcept
		{
			return std::clamp<int32_t>(base * (j + 1) - bias, tmin, tmax);
		}

		[[nodiscard]] constexpr uint32_t w(uint32_t j) noexcept
		{
			if (j == 0)
			{
				return 1;
			}
			return w(j - 1) * (base - t(j - 1));
		}

		[[nodiscard]] static char encodeDigit(uint32_t d)
		{
			if (d < 26)
			{
				return 'a' + d;
			}
			return '0' + (d - 26);
		}
		
		[[nodiscard]] static uint32_t decodeDigit(char d)
		{
			if (d >= 'a' && d <= 'z')
			{
				return d - 'a';
			}
			if (d >= '0' && d <= '9')
			{
				return (d - '0') + 26;
			}
			return d - 'A';
		}

		void updateBias(uint32_t delta, uint32_t numpoints)
		{
			if (numpoints == 1) // First decoded delta?
			{
				delta /= damp;
			}
			else
			{
				delta /= 2;
			}
			delta += (delta / numpoints);
			uint32_t k = 0;
			while (delta > (((base - tmin) * tmax) / 2))
			{
				delta /= (base - tmin);
				k += base;
			}
			bias = k + (((base - tmin + 1) * delta) / (delta + skew));
		}

		[[nodiscard]] std::string encodeDeltas(const std::vector<uint32_t>& deltas)
		{
			std::string encoded;
			for (uint32_t h = 0; h != deltas.size(); )
			{
				auto delta = deltas.at(h);
				auto q = delta;
				for (uint32_t j = 0;; ++j)
				{
					const auto u = t(j);
					if (q < u)
					{
						break;
					}
					encoded.push_back(encodeDigit(u + ((q - u) % (base - u))));
					q = (q - u) / (base - u);
				}
				encoded.push_back(encodeDigit(q));
				updateBias(delta, ++h);
			}
			return encoded;
		}

		[[nodiscard]] std::vector<uint32_t> decodeDeltas(const char* data)
		{
			std::vector<uint32_t> ret;
			uint32_t j = 0;
			uint32_t delta = 0;
			for (; *data != '\0'; ++data)
			{
				auto digit = decodeDigit(*data);
				//std::cout << "char = " << *data << ", value = " << digit << ", t(j) = " << t(j) << ", w(j) = " << w(j) << "\n";
				if (digit != 0)
				{
					delta += (digit * w(j));
				}
				if (digit < t(j))
				{
					ret.emplace_back(delta);
					updateBias(delta, ret.size());
					//std::cout << "decoded delta = " << delta << ", new bias = " << bias << "\n";
					j = 0;
					delta = 0;
				}
				else
				{
					++j;
				}
			}
			return ret;
		}

		[[nodiscard]] std::string encode(const std::u32string& in)
		{
			std::string out;
			std::vector<uint32_t> deltas;

			uint32_t num_non_basic = 0;
			out.reserve(in.size());
			for (const uint32_t c : in)
			{
				if (c < 128)
				{
					out.push_back(c);
				}
				else
				{
					++num_non_basic;
				}
			}
			
			if (!out.empty())
			{
				out.push_back('-');
			}

			uint32_t n = initial_n;
			uint32_t delta = 0;
			for (uint32_t h = 0; h != num_non_basic; ++delta, ++n)
			{
				uint32_t m = n;
				for (const uint32_t c : in)
				{
					if (c >= n && c < m)
					{
						m = c;
					}
				}
				delta += (m - n) * (h + 1);
				n = m;
				for (const uint32_t c : in)
				{
					if (c < n)
					{
						++delta;
					}
					else if (c == n)
					{
						deltas.emplace_back(delta);
						delta = 0;
						++h;
					}
				}
			}

			return out + encodeDeltas(deltas);
		}

		[[nodiscard]] std::u32string decode(const std::string& in)
		{
			std::u32string str;
			std::vector<uint32_t> deltas;

			auto del = in.find_last_of('-');
			if (del != std::string::npos)
			{
				str.reserve(del);
				for (uint32_t i = 0; i != del; ++i)
				{
					str.push_back(in.at(i));
				}
				deltas = decodeDeltas(in.c_str() + del + 1);
			}
			else
			{
				deltas = decodeDeltas(in.c_str());
			}

			uint32_t i = 0;
			uint32_t n = initial_n;
			uint32_t size = str.size();
			for (auto delta : deltas)
			{
				//std::cout << "delta = " << delta << "\n";

#if true
				const uint32_t insertion_points = (size + 1);
				//std::cout << "insertion_points = " << insertion_points << "\n";
				const uint32_t q = (delta / insertion_points);
				const uint32_t r = (delta % insertion_points);
				i = (i + r);
				n = (n + q);
				if (i >= insertion_points)
				{
					i -= insertion_points;
					++n;
				}
#else
				while (delta--)
				{
					if (i == size)
					{
						i = 0;
						++n;
					}
					else
					{
						++i;
					}
				}
#endif
				//std::cout << "i = " << i << "\n";
				//std::cout << "n = " << n << "\n";

				str.insert(i, 1, n);
				++size;
				if (i == size)
				{
					i = 0;
					++n;
				}
				else
				{
					++i;
				}
			}
			return str;
		}
	};

	std::string punycode::encode(const std::u32string& in)
	{
		PunycodeState st;
		return st.encode(in);
	}

	std::u32string punycode::decode(const std::string& in)
	{
		PunycodeState st;
		return st.decode(in);
	}

	std::string punycode::encodeDeltas(const std::vector<uint32_t>& deltas)
	{
		PunycodeState st;
		return st.encodeDeltas(deltas);
	}

	std::vector<uint32_t> punycode::decodeDeltas(const char* data)
	{
		PunycodeState st;
		return st.decodeDeltas(data);
	}
}
