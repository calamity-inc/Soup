#pragma once

#include "vector_maths.hpp"
#include "vector3.hpp"

namespace soup
{
#pragma pack(push, 1)
#pragma warning(push)
#pragma warning(disable: 26495) // uninitialised member variable
	class matrix
	{
	public:
		float mf[16];

		matrix(const bool bIdentity = true)
		{
			if (bIdentity) setToIdentity();
		}

		matrix(const vector3& pos, const vector3& rot)
			: matrix(true)
		{
			setTranslate(pos);
			setRotation(rot);
		}

		void setToIdentity()
		{
			mf[ 0] = 1.0f;	mf[ 1] = 0.0f;	mf[ 2] = 0.0f;	mf[ 3] = 0.0f;
			mf[ 4] = 0.0f;	mf[ 5] = 1.0f;	mf[ 6] = 0.0f;	mf[ 7] = 0.0f;
			mf[ 8] = 0.0f;	mf[ 9] = 0.0f;	mf[10] = 1.0f;	mf[11] = 0.0f;
			mf[12] = 0.0f;	mf[13] = 0.0f;	mf[14] = 0.0f;	mf[15] = 1.0f;
		}

		// Concatenate 2 matrices with the * operator
		inline matrix operator* (const matrix& InM) const
		{
			matrix res(false);
			for (int i = 0; i < 16; i += 4)
			{
				for (int j = 0; j < 4; j++)
				{
					res.mf[i + j] = (mf[i + 0] * InM.mf[0 + j] + mf[i + 1] * InM.mf[4 + j] + mf[i + 2] * InM.mf[8 + j] + mf[i + 3] * InM.mf[12 + j]);
				}
			}
			return res;
		}

		// Use a matrix to transform a 3D point with the * operator
		inline vector3 operator* (const vector3& Point) const
		{
			float x = Point.x * mf[0] + Point.y * mf[4] + Point.z * mf[8] + mf[12];
			float y = Point.x * mf[1] + Point.y * mf[5] + Point.z * mf[9] + mf[13];
			float z = Point.x * mf[2] + Point.y * mf[6] + Point.z * mf[10] + mf[14];
			return vector3{ x, y, z };
		}

		void resetRotation()
		{
			mf[0] = 1.0f;
			mf[1] = 0.0f;
			mf[2] = 0.0f;
			mf[4] = 0.0f;
			mf[5] = 1.0f;
			mf[6] = 0.0f;
			mf[8] = 0.0f;
			mf[9] = 0.0f;
			mf[10] = 1.0f;
		}

		void setRotation(const vector3& rot)
		{
			resetRotation();
			rotate(rot.x, true, false, false);
			rotate(rot.y, false, true, false);
			rotate(rot.z + 90.0f, false, false, true);
		}

		void rotate(const vector3& rot)
		{
			rotate(rot.x, true, false, false);
			rotate(rot.y, false, true, false);
			rotate(rot.z, false, false, true);
		}

		void rotateCCW(const vector3& rot)
		{
			rotate(rot * -1.0f);
		}

		// Rotate the *this matrix fDegrees clockwise around a single axis( either x, y, or z )
		void rotate(float fDegrees, bool x, bool y, bool z)
		{
			matrix tmp;
			if (x) tmp.rotateImplX(fDegrees);
			if (y) tmp.rotateImplY(fDegrees);
			if (z) tmp.rotateImplZ(fDegrees);
			*this = tmp * (*this);
		}

	private:
		void rotateImplX(float angle)
		{
			mf[5] = cosf(DEG_TO_RAD(angle));
			mf[6] = sinf(DEG_TO_RAD(angle));
			mf[9] = -sinf(DEG_TO_RAD(angle));
			mf[10] = cosf(DEG_TO_RAD(angle));
		}
		void rotateImplY(float angle)
		{
			mf[0] = cosf(DEG_TO_RAD(angle));
			mf[2] = -sinf(DEG_TO_RAD(angle));
			mf[8] = sinf(DEG_TO_RAD(angle));
			mf[10] = cosf(DEG_TO_RAD(angle));
		}
		void rotateImplZ(float angle)
		{
			mf[0] = cosf(DEG_TO_RAD(angle));
			mf[1] = sinf(DEG_TO_RAD(angle));
			mf[4] = -sinf(DEG_TO_RAD(angle));
			mf[5] = cosf(DEG_TO_RAD(angle));
		}

	public:
		void scale(float sx, float sy, float sz)
		{
			int x;
			for (x = 0; x < 4; x++) mf[x] *= sx;
			for (x = 4; x < 8; x++) mf[x] *= sy;
			for (x = 8; x < 12; x++) mf[x] *= sz;
		}

		void translate(const vector3& test)
		{
			for (int j = 0; j < 4; j++)
			{
				mf[12 + j] += test.x * mf[j] + test.y * mf[4 + j] + test.z * mf[8 + j];
			}
		}

		[[nodiscard]] vector3 getTranslate() const noexcept
		{
			return vector3{ mf[12], mf[13], mf[14] };
		}

		void setTranslate(const vector3& b)
		{
			mf[12] = b.x;
			mf[13] = b.y;
			mf[14] = b.z;
		}

		void resetTranslate()
		{
			mf[12] = 0;
			mf[13] = 0;
			mf[14] = 0;
		}

		// Create a rotation matrix for a counter-clockwise rotation of fDegrees around an arbitrary axis(x, y, z)
		void rotateMatrix(float fDegrees, float x, float y, float z)
		{
			setToIdentity();
			float cosA = cosf(DEG_TO_RAD(fDegrees));
			float sinA = sinf(DEG_TO_RAD(fDegrees));
			float m = 1.0f - cosA;
			mf[0] = cosA + x * x * m;
			mf[5] = cosA + y * y * m;
			mf[10] = cosA + z * z * m;

			float tmp1 = x * y * m;
			float tmp2 = z * sinA;
			mf[4] = tmp1 + tmp2;
			mf[1] = tmp1 - tmp2;

			tmp1 = x * z * m;
			tmp2 = y * sinA;
			mf[8] = tmp1 - tmp2;
			mf[2] = tmp1 + tmp2;

			tmp1 = y * z * m;
			tmp2 = x * sinA;
			mf[9] = tmp1 + tmp2;
			mf[6] = tmp1 - tmp2;
		}

		// Simple but not robust matrix inversion. (Doesn't work properly if there is a scaling or skewing transformation.)
		inline matrix invertSimple()
		{
			matrix R(0);
			R.mf[0] = mf[0]; 		R.mf[1] = mf[4];		R.mf[2] = mf[8];	R.mf[3] = 0.0f;
			R.mf[4] = mf[1];		R.mf[5] = mf[5];		R.mf[6] = mf[9];	R.mf[7] = 0.0f;
			R.mf[8] = mf[2];		R.mf[9] = mf[6];		R.mf[10] = mf[10];	R.mf[11] = 0.0f;
			R.mf[12] = -(mf[12] * mf[0]) - (mf[13] * mf[1]) - (mf[14] * mf[2]);
			R.mf[13] = -(mf[12] * mf[4]) - (mf[13] * mf[5]) - (mf[14] * mf[6]);
			R.mf[14] = -(mf[12] * mf[8]) - (mf[13] * mf[9]) - (mf[14] * mf[10]);
			R.mf[15] = 1.0f;
			return R;
		}

		// Invert for only a rotation, any translation is zeroed out
		matrix invertRot()
		{
			matrix R(false);
			R.mf[0] = mf[0]; 		R.mf[1] = mf[4];		R.mf[2] = mf[8];	R.mf[3] = 0.0f;
			R.mf[4] = mf[1];		R.mf[5] = mf[5];		R.mf[6] = mf[9];	R.mf[7] = 0.0f;
			R.mf[8] = mf[2];		R.mf[9] = mf[6];		R.mf[10] = mf[10];	R.mf[11] = 0.0f;
			R.mf[12] = 0;			R.mf[13] = 0;			R.mf[14] = 0;		R.mf[15] = 1.0f;
			return R;
		}
	};
#pragma warning(pop)
#pragma pack(pop)
}
