#include "ecc.hpp"

#include "rand.hpp"

namespace soup
{
	using namespace literals;

	EccCurve EccCurve::secp256r1()
	{
		// https://asecuritysite.com/ecc/p256p
		EccCurve curve;
		curve.a = "-3"_b;
		curve.b = "41058363725152142129326129780047268409114441015993725554835256314039467401291"_b;
		curve.p = "0xFFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFF"_b; // Bigint::_2pow(256) - Bigint::_2pow(224) + Bigint::_2pow(192) + Bigint::_2pow(96) - "1"_b
		curve.G = EccPoint(
			"48439561293906451759052585252797914202762949526041747995844080717082404635286"_b,
			"36134250956749795798585127919587881956611106672985015071877198253568414405109"_b
		);
		curve.n = "0xFFFFFFFF00000000FFFFFFFFFFFFFFFFBCE6FAADA7179E84F3B9CAC2FC632551"_b;
		return curve;
	}

	EccCurve EccCurve::secp384r1()
	{
		// https://asecuritysite.com/ecc/ecc_points2, https://www.secg.org/sec2-v2.pdf
		EccCurve curve;
		curve.a = "-3"_b;
		curve.b = "27580193559959705877849011840389048093056905856361568521428707301988689241309860865136260764883745107765439761230575"_b;
		curve.p = "0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFF0000000000000000FFFFFFFF"_b; // Bigint::_2pow(384) - Bigint::_2pow(128) - Bigint::_2pow(96) + Bigint::_2pow(32) - "1"_b
		curve.G = EccPoint(
			"0xAA87CA22BE8B05378EB1C71EF320AD746E1D3B628BA79B9859F741E082542A385502F25DBF55296C3A545E3872760AB7"_b,
			"0x3617DE4A96262C6F5D9E98BF9292DC29F8F41DBD289A147CE9DA3113B5F0B8C00A60B1CE1D7E819D7A431D7C90EA0E5F"_b
		);
		curve.n = "0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC7634D81F4372DDF581A0DB248B0A77AECEC196ACCC52973"_b;
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

	bool EccCurve::validate(const EccPoint& P) const
	{
		return (P.y.pow2() % this->p) == (((P.x * P.x * P.x) + (this->a * P.x) + this->b) % this->p);
	}
}
