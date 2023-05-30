#include "ecc.hpp"

#include "rand.hpp"

namespace soup
{
	using namespace literals;

	EccCurve EccCurve::secp256r1()
	{
		EccCurve curve;
		curve.a = "-3"_b;
		curve.b = "41058363725152142129326129780047268409114441015993725554835256314039467401291"_b;
		curve.p = "115792089210356248762697446949407573530086143415290314195533631308867097853951"_b;
		curve.G = EccPoint(
			"48439561293906451759052585252797914202762949526041747995844080717082404635286"_b,
			"36134250956749795798585127919587881956611106672985015071877198253568414405109"_b
		);
		curve.n = "115792089210356248762697446949407573529996955224135760342422259061068512044369"_b;
		return curve;
	}

	Bigint EccCurve::generatePrivate() const
	{
		Bigint d;
		while (d < this->n)
		{
			d.setBit(0, rand.coinflip());
			d <<= 1;
		}
		d >>= 1;
		return d;
	}

	EccPoint EccCurve::derivePublic(const Bigint& d) const
	{
		return multiply(this->G, d);
	}

	EccPoint EccCurve::add(const EccPoint& P, const EccPoint& Q) const
	{
		if (P.point_at_infinity)
		{
			// 0 + Q = Q
			return Q;
		}
		if (Q.point_at_infinity)
		{
			// P + 0 = P
			return P;
		}
		if (P.x == Q.x && P.y != Q.y)
		{
			// point1 + (-point1) = 0
			return EccPoint();
		}
		Bigint m;
		if (P.x == Q.x)
		{
			m = (3_b * P.x.pow2() + this->a) * (2_b * P.y).modMulInv(this->p);
		}
		else
		{
			m = (P.y - Q.y) * (P.x - Q.x).modMulInv(this->p);
		}
		Bigint x = (m.pow2() - P.x - Q.x) % this->p;
		Bigint y = (m * (P.x - x) - P.y) % this->p;
		return EccPoint(std::move(x), std::move(y));
	}

	EccPoint EccCurve::multiply(EccPoint G, Bigint d) const
	{
#if true
		// Double-and-add
		EccPoint R;
		while (!d.isZero())
		{
			if (d.isOdd())
			{
				R = add(R, G);
			}
			G = add(G, G);
			d >>= 1;
		}
		return R;
#else
		// Montgomery ladder (constant-time)
		EccPoint R0, R1 = G;
		for (int i = d.getBitLength() - 1; i >= 0; i--)
		{
			if (d.getBit(i) == 0)
			{
				R1 = add(R0, R1);
				R0 = add(R0, R0);
			}
			else
			{
				R0 = add(R0, R1);
				R1 = add(R1, R1);
			}
		}
		return R0;

#endif
	}
}
