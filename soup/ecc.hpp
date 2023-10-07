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

		[[nodiscard]] bool isIdentityElement() const noexcept
		{
			return x.isZero() && y.isZero();
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

		[[nodiscard]] static const EccCurve& secp256k1();
		[[nodiscard]] static const EccCurve& secp256r1(); // aka. P-256
		[[nodiscard]] static const EccCurve& secp384r1(); // aka. P-384

		[[nodiscard]] Bigint generatePrivate() const;
		[[nodiscard]] EccPoint derivePublic(const Bigint& d) const;

		[[nodiscard]] EccPoint add(const EccPoint& P, const EccPoint& Q) const;
		[[nodiscard]] EccPoint multiply(EccPoint G, Bigint d) const;

		[[nodiscard]] std::string encodePointUncompressed(const EccPoint& P) const;
		[[nodiscard]] std::string encodePointCompressed(const EccPoint& P) const;

		[[nodiscard]] EccPoint decodePoint(const std::string& str) const;

		// Checks if P satisfies y^2 = x^3 + ax + b (mod p)
		// A valid curve point also satisfies multiply(P, n).isIdentityElement()
		[[nodiscard]] bool validate(const EccPoint& P) const;

		[[nodiscard]] size_t getBytesPerAxis() const;

		// ECDSA
		[[nodiscard]] std::pair<Bigint, Bigint> sign(const Bigint& d, const std::string& hash) const; // (r, s)
		[[nodiscard]] bool verify(const EccPoint& pub, const std::string& hash, const Bigint& r, const Bigint& s) const;
		[[nodiscard]] Bigint deriveD(const std::string& e1, const std::string& e2, const Bigint& r, const Bigint& s1, const Bigint& s2) const;
		[[nodiscard]] Bigint e2z(const std::string& e) const;
	};
}
