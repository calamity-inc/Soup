#include "FactoriseProofOfWork.hpp"

#include "Bigint.hpp"
#include "string.hpp"

NAMESPACE_SOUP
{
	using namespace literals;

	std::string FactoriseProofOfWork::createChallenge(uint8_t difficulty)
	{
		Bigint p = Bigint::randomProbablePrime(difficulty);
		Bigint q = Bigint::randomProbablePrime(difficulty);
		return (p * q).toStringHex();
	}

	std::string FactoriseProofOfWork::solve(const std::string& challenge)
	{
		auto n = Bigint::fromStringHex(challenge.data(), challenge.size());
		auto [p, q] = n.factorise();
		std::string str = p.toStringHex();
		str.push_back('.');
		str.append(q.toStringHex());
		return str;
	}

	bool FactoriseProofOfWork::verify(const std::string& challenge, const std::string& solution)
	{
		auto n = Bigint::fromStringHex(challenge.data(), challenge.size());
		auto arr = string::explode(solution, '.');
		if (arr.size() == 2)
		{
			auto p = Bigint::fromStringHex(arr.at(0).data(), arr.at(0).size());
			auto q = Bigint::fromStringHex(arr.at(1).data(), arr.at(1).size());
			if (p != 1_b
				//&& p != n
				&& q != 1_b
				//&& q != n
				)
			{
				return (p * q) == n;
			}
		}
		return false;
	}
}
