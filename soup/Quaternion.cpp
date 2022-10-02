#include "Quaternion.hpp"

#include "Matrix.hpp"
#include "Vector3.hpp"

namespace soup
{
	Quaternion::Quaternion(float angle, const Vector3& axis) noexcept
	{
		setAxisAngle(angle, axis.x, axis.y, axis.z);
	}

	void Quaternion::setAxisAngle(float degrees, float fX, float fY, float fZ) noexcept
	{
		float HalfAngle = DEG_TO_HALF_RAD(degrees); // Get half angle in radians from angle in degrees
		float sinA = sinf(HalfAngle);
		w = cosf(HalfAngle);
		x = fX * sinA;
		y = fY * sinA;
		z = fZ * sinA;
	}

	Quaternion Quaternion::fromEuler(const Vector3& rot) noexcept
	{
		return fromEulerXYZ(rot.y, -rot.x, rot.z + 90.0f);
	}

	Quaternion Quaternion::fromEulerZYX(const Vector3& rot) noexcept
	{
		return fromEulerXYZ(-rot.x, rot.y, -rot.z);
	}

	Quaternion Quaternion::fromEulerXYZ(const Vector3& rot) noexcept
	{
		return fromEulerXYZ(rot.x, rot.y, rot.z);
	}

	Quaternion Quaternion::fromEulerXYZ(float rx, float ry, float rz) noexcept
	{
		Quaternion qx(-rx, Vector3(1, 0, 0));
		Quaternion qy(-ry, Vector3(0, 1, 0));
		Quaternion qz(-rz, Vector3(0, 0, 1));
		qz = qy * qz;
		return qx * qz;
	}

	Quaternion Quaternion::invert() const noexcept
	{
		return Quaternion{ -x, -y, -z, w };
	}

	Quaternion Quaternion::operator+(const Quaternion& b) const noexcept
	{
		return Quaternion{ x + b.x, y + b.y, z + b.z, w + b.w };
	}

	Quaternion Quaternion::operator*(const Quaternion& b) const
	{
		Quaternion r;
		r.w = w * b.w - x * b.x - y * b.y - z * b.z;
		r.x = w * b.x + x * b.w + y * b.z - z * b.y;
		r.y = w * b.y + y * b.w + z * b.x - x * b.z;
		r.z = w * b.z + z * b.w + x * b.y - y * b.x;
		return r;
	}

	Quaternion Quaternion::operator*(float s) const noexcept
	{
		return Quaternion{ x * s, y * s, z * s, w * s };
	}

	bool Quaternion::operator==(const Quaternion& b) const noexcept
	{
		return (x == b.x && y == b.y && z == b.z && w == b.w);
	}

	bool Quaternion::isIdentity() const noexcept
	{
		return (x == 0.0f && y == 0.0f && z == 0.0f && w == 1.0f);
	}

	void Quaternion::reset() noexcept
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 1.0f;
	}

	int Quaternion::normalise() noexcept
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

	Quaternion Quaternion::slerp(const Quaternion& a, const Quaternion& b, float t, const bool bReduceTo360) noexcept
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

	Quaternion Quaternion::nlerp(const Quaternion& a, const Quaternion& b, float t, const bool bReduceTo360) noexcept
	{
		Quaternion res{};
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

	void Quaternion::toMatrix(Matrix& m) const noexcept
	{
		toMatrix(m.mf);
	}

	void Quaternion::toMatrix(float mf[16]) const noexcept
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

	void Quaternion::aimZAxis(const Vector3& P1, const Vector3& P2) noexcept
	{
		Vector3 vAim = P2 - P1;
		vAim.normalise();

		x = vAim.y;
		y = -vAim.x;
		z = 0.0f;
		w = 1.0f + vAim.z;

		if (x == 0.0f && y == 0.0f && z == 0.0f && w == 0.0f)
		{
			*this = Quaternion{ 0, 1, 0, 0 }; // If we can't normalize it, just set it
		}
		else
		{
			normalise();
		}
	}
}
