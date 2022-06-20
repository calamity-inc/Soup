#include "Bigfloat.hpp"

namespace soup
{
	Bigfloat Bigfloat::operator+(const Bigfloat& b) const noexcept
	{
		return {
			i + b.i,
			f + b.f,
		};
	}

	std::string Bigfloat::toString() const noexcept
	{
		std::string str = i.toString();
		str.push_back('.');
		str.append(f.toString());
		return str;
	}
}
