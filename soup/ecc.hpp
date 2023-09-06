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

		[[nodiscard]] std::string toBinary(size_t bytes_per_axis) const
		{
			return x.toBinary(bytes_per_axis) + y.toBinary(bytes_per_axis);
		}

		friend struct EccCurve;
	};

	struct EccCurve
	{
		Bigint a;
		Bigint b;
		Bigint p;
		EccPoint G;
		Bigint n;

		[[nodiscard]] static EccCurve secp256k1();
		[[nodiscard]] static EccCurve secp256r1();
		[[nodiscard]] static EccCurve secp384r1();

		[[nodiscard]] Bigint generatePrivate() const;
		[[nodiscard]] EccPoint derivePublic(const Bigint& d) const;

		[[nodiscard]] EccPoint add(const EccPoint& P, const EccPoint& Q) const;
		[[nodiscard]] EccPoint multiply(EccPoint G, Bigint d) const;

		// Checks if P satifies y^2 = x^3 + ax + b (mod p)
		[[nodiscard]] bool validate(const EccPoint& P) const;

		[[nodiscard]] size_t getBytesPerAxis() const;
	};
}
