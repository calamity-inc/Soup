#include "quaternion.hpp"

#include "matrix.hpp"
#include "vector3.hpp"

namespace soup
{
	quaternion::quaternion(float angle, const vector3& axis) noexcept
	{
		setAxisAngle(angle, axis.x, axis.y, axis.z);
	}

	void quaternion::setAxisAngle(float degrees, float fX, float fY, float fZ) noexcept
	{
		float HalfAngle = DEG_TO_HALF_RAD(degrees); // Get half angle in radians from angle in degrees
		float sinA = sinf(HalfAngle);
		w = cosf(HalfAngle);
		x = fX * sinA;
		y = fY * sinA;
		z = fZ * sinA;
	}

	quaternion quaternion::fromEuler(const vector3& rot) noexcept
	{
		return fromEuler(rot.y, -rot.x, rot.z + 90.0f);
	}

	quaternion quaternion::fromEulerZYX(const vector3& rot) noexcept
	{
		return fromEuler(-rot.x, rot.y, -rot.z);
	}

	quaternion quaternion::fromEuler(float rx, float ry, float rz) noexcept
	{
		quaternion qx(-rx, vector3(1, 0, 0));
		quaternion qy(-ry, vector3(0, 1, 0));
		quaternion qz(-rz, vector3(0, 0, 1));
		qz = qy * qz;
		return qx * qz;
	}

	quaternion quaternion::invert() const noexcept
	{
		return quaternion{ -x, -y, -z, w };
	}

	quaternion quaternion::operator+(const quaternion& b) const noexcept
	{
		return quaternion{ x + b.x, y + b.y, z + b.z, w + b.w };
	}

	quaternion quaternion::operator*(const quaternion& b) const
	{
		quaternion r;
		r.w = w * b.w - x * b.x - y * b.y - z * b.z;
		r.x = w * b.x + x * b.w + y * b.z - z * b.y;
		r.y = w * b.y + y * b.w + z * b.x - x * b.z;
		r.z = w * b.z + z * b.w + x * b.y - y * b.x;
		return r;
	}

	quaternion quaternion::operator*(float s) const noexcept
	{
		return quaternion{ x * s, y * s, z * s, w * s };
	}

	bool quaternion::operator==(const quaternion& b) const noexcept
	{
		return (x == b.x && y == b.y && z == b.z && w == b.w);
	}

	bool quaternion::isIdentity() const noexcept
	{
		return (x == 0.0f && y == 0.0f && z == 0.0f && w == 1.0f);
	}

	void quaternion::reset() noexcept
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 1.0f;
	}

	int quaternion::normalise() noexcept
	{
		float lengthSq = x * x + y * y + z * z + w * w;

		if (lengthSq == 0.0) return -1;
		if (lengthSq != 1.0)
		{
			float scale = (1.0f / sqrtf(lengthSq));
			x *= scale;
			y *= scale;
			z *= scale;
			w *= scale;
			return 1;
		}
		return 0;
	}

	quaternion quaternion::slerp(const quaternion& a, const quaternion& b, float t, const bool bReduceTo360) noexcept
	{
		float w1, w2;
		bool bFlip = false;

		float cosTheta = a.dot(b);
		if (bReduceTo360 && cosTheta < 0.0f)
		{
			// We need to flip a quaternion for shortest path interpolation
			cosTheta = -cosTheta;
			bFlip = true;
		}
		float theta = acos(cosTheta);
		float sinTheta = sin(theta);

		if (sinTheta <= 0.005f)
		{
			// They're almost the same quaternion
			w1 = 1.0f - t;
			w2 = t;
		}
		else
		{
			w1 = sin((1.0f - t) * theta) / sinTheta;
			w2 = sin(t * theta) / sinTheta;
		}

		if (bFlip)
		{
			w2 = -w2;
		}

		return a * w1 + b * w2;
	}

	quaternion quaternion::nlerp(const quaternion& a, const quaternion& b, float t, const bool bReduceTo360) noexcept
	{
		quaternion res{};
		float t1 = 1.0f - t;
		if (bReduceTo360 && a.dot(b) < 0.0f)
		{
			res = a * t1 + b * -t;
		}
		else
		{
			res = a * t1 + b * t;
		}
		res.normalise();
		return res;
	}

	void quaternion::toMatrix(matrix& m) const noexcept
	{
		toMatrix(m.mf);
	}

	void quaternion::toMatrix(float mf[16]) const noexcept
	{
		float x2 = 2.0f * x, y2 = 2.0f * y, z2 = 2.0f * z;

		float xy = x2 * y, xz = x2 * z;
		float yy = y2 * y, yw = y2 * w;
		float zw = z2 * w, zz = z2 * z;

		mf[0] = 1.0f - (yy + zz);
		mf[1] = (xy - zw);
		mf[2] = (xz + yw);
		mf[3] = 0.0f;

		float xx = x2 * x, xw = x2 * w, yz = y2 * z;

		mf[4] = (xy + zw);
		mf[5] = 1.0f - (xx + zz);
		mf[6] = (yz - xw);
		mf[7] = 0.0f;

		mf[8] = (xz - yw);
		mf[9] = (yz + xw);
		mf[10] = 1.0f - (xx + yy);
		mf[11] = 0.0f;

		//mf[12] = 0.0f;
		//mf[13] = 0.0f;
		//mf[14] = 0.0f;
		//mf[15] = 1.0f;
	}

	void quaternion::aimZAxis(const vector3& P1, const vector3& P2) noexcept
	{
		vector3 vAim = P2 - P1;
		vAim.normalise();

		x = vAim.y;
		y = -vAim.x;
		z = 0.0f;
		w = 1.0f + vAim.z;

		if (x == 0.0f && y == 0.0f && z == 0.0f && w == 0.0f)
		{
			*this = quaternion{ 0, 1, 0, 0 }; // If we can't normalize it, just set it
		}
		else
		{
			normalise();
		}
	}
}
