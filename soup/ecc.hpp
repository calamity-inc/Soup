#pragma once

#include "Bigint.hpp"

namespace soup
{

	struct EccPoint
	{
	protected:
		bool point_at_infinity = true;
		Bigint x{};
		Bigint y{};

	public:
		EccPoint() = default;

		EccPoint(Bigint x, Bigint y)
			: point_at_infinity(false), x(std::move(x)), y(std::move(y))
		{
		}

		[[nodiscard]] const Bigint& getX() const
		{
			return x;
		}

		[[nodiscard]] const Bigint& getY() const
		{
			return y;
		}

		[[nodiscard]] std::string toBinary() const;

		friend struct EccCurve;
	};

	// y^2 = x^3 + ax + b (mod p)
	struct EccCurve
	{
		Bigint a;
		Bigint b;
		Bigint p;
		EccPoint G;
		Bigint n;

		[[nodiscard]] static EccCurve secp256r1();

		[[nodiscard]] Bigint generatePrivate() const;
		[[nodiscard]] EccPoint derivePublic(const Bigint& d) const;

		[[nodiscard]] EccPoint add(const EccPoint& P, const EccPoint& Q) const;
		[[nodiscard]] EccPoint multiply(EccPoint G, Bigint d) const;
	};
}
