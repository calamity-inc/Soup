#pragma once

#include "vector4.hpp"

#include "vector3.hpp"

namespace soup
{
#pragma pack(push, 1)
	struct quaternion : public vector4
	{
		using vector4::vector4;

		// This just took four floats initially to avoid dependence on the vector class
		// but I decided avoiding confusion with the value setting constructor was more important
		quaternion(float Angle, const vector3& Axis)
		{
			setAxis(Angle, Axis.x, Axis.y, Axis.z);
		}

		void reset()
		{
			x = 0;
			y = 0;
			z = 0;
			w = 1;
		}

		// Set Quat from axis-angle
		void setAxis(float degrees, float fX, float fY, float fZ)
		{
			float HalfAngle = DEG_TO_HALF_RAD(degrees); // Get half angle in radians from angle in degrees
			float sinA = (float)sin(HalfAngle);
			w = (float)cos(HalfAngle);
			x = fX * sinA;
			y = fY * sinA;
			z = fZ * sinA;
		}

		[[nodiscard]] quaternion invert() const
		{
			return quaternion{ -x, -y, -z, w };
		}

		// Note that order matters with concatenating Quaternion rotations
		inline quaternion operator* (const quaternion& b) const
		{
			quaternion r;

			r.w = w * b.w - x * b.x - y * b.y - z * b.z;
			r.x = w * b.x + x * b.w + y * b.z - z * b.y;
			r.y = w * b.y + y * b.w + z * b.x - x * b.z;
			r.z = w * b.z + z * b.w + x * b.y - y * b.x;

			return r;
		}

		// You could add an epsilon to this equality test if needed
		inline bool operator== (const quaternion& b) const
		{
			return (x == b.x && y == b.y && z == b.z && w == b.w);
		}

		int isIdentity() const
		{
			return (x == 0.0f && y == 0.0f && z == 0.0f && w == 1.0f);
		}

		// Scalar multiplication
		quaternion operator*(float s) const
		{
			return quaternion{ x * s, y * s, z * s, w * s };
		}

		// Addition
		quaternion operator+ (const quaternion& b) const
		{
			return quaternion{ x + b.x, y + b.y, z + b.z, w + b.w };
		}

		// ------------------------------------
		// Simple Euler Angle to Quaternion conversion, this could be made faster
		// ------------------------------------
		void fromEuler(float rx, float ry, float rz)
		{
			quaternion qx(-rx, vector3(1, 0, 0));
			quaternion qy(-ry, vector3(0, 1, 0));
			quaternion qz(-rz, vector3(0, 0, 1));
			qz = qy * qz;
			*this = qx * qz;
		}

		// ------------------------------------
		// Quaternions store scale as well as rotation, but usually we just want rotation, so we can normalize.
		// ------------------------------------
		int normalise()
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

		// ------------------------------------
		// Creates a value for this Quaternion from spherical linear interpolation
		// t is the interpolation value from 0 to 1
		// if bReduceTo360 is true, the interpolation will take the shortest path for a 360 deg angle range (max delta rotation = 180 degrees)
		// if bReduceTo360 is false, the interpolation will take the shortest path for a 720 deg angle range (max delta rotation = 360 degrees)
		// ------------------------------------
		void slerp(const quaternion& a, const quaternion& b, float t, const bool bReduceTo360)
		{
			float w1, w2;
			int bFlip = 0;

			float cosTheta = a.dot(b);
			if (bReduceTo360 && cosTheta < 0.0f) { // We need to flip a quaternion for shortest path interpolation
				cosTheta = -cosTheta;
				bFlip = 1;
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
				w2 = -w2;

			*this = a * w1 + b * w2;
		}

		// ------------------------------------
		// linearly interpolate each component, then normalize the Quaternion
		// Unlike spherical interpolation, this does not rotate at a constant velocity,
		// although that's not necessarily a bad thing
		// ------------------------------------
		void nlerp(const quaternion& a, const quaternion& b, float t, const bool bReduceTo360)
		{
			float t1 = 1.0f - t;

			if (bReduceTo360 && a.dot(b) < 0.0f)
				*this = a * t1 + b * -t;
			else
				*this = a * t1 + b * t;

			normalise();
		}

		// ------------------------------------
		// Set a 4x4 matrix with the rotation of this Quaternion
		// ------------------------------------
		void inline toMatrix(float mf[16]) const
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

			mf[12] = 0.0f;
			mf[13] = 0.0f;
			mf[14] = 0.0f;
			mf[15] = 1.0f;
		}

		// ------------------------------------
		// Set this Quat to aim the Z-Axis along the vector from P1 to P2
		// ------------------------------------
		void aimZAxis(const vector3& P1, const vector3& P2)
		{
			vector3 vAim = P2 - P1;
			vAim.normalize();

			x = vAim.y;
			y = -vAim.x;
			z = 0.0f;
			w = 1.0f + vAim.z;

			if (x == 0.0f && y == 0.0f && z == 0.0f && w == 0.0f) {
				*this = quaternion{ 0, 1, 0, 0 }; // If we can't normalize it, just set it
			}
			else {
				normalise();
			}
		}

	};
#pragma pack(pop)
}
