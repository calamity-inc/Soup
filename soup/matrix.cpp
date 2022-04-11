#include "matrix.hpp"

#include "quaternion.hpp"
#include "vector_maths.hpp"
#include "vector3.hpp"

namespace soup
{
	matrix::matrix() noexcept
	{
		reset();
	}

	matrix::matrix(const vector3& pos, const vector3& rot) noexcept
	{
		setTranslate(pos);
		setRotation(rot);
	}

	void matrix::reset() noexcept
	{
		mf[0] = 1.0f;	mf[1] = 0.0f;	mf[2] = 0.0f;	mf[3] = 0.0f;
		mf[4] = 0.0f;	mf[5] = 1.0f;	mf[6] = 0.0f;	mf[7] = 0.0f;
		mf[8] = 0.0f;	mf[9] = 0.0f;	mf[10] = 1.0f;	mf[11] = 0.0f;
		mf[12] = 0.0f;	mf[13] = 0.0f;	mf[14] = 0.0f;	mf[15] = 1.0f;
	}

	matrix matrix::operator*(const matrix& InM) const noexcept
	{
		matrix res;
		for (int i = 0; i < 16; i += 4)
		{
			for (int j = 0; j < 4; j++)
			{
				res.mf[i + j] = (mf[i + 0] * InM.mf[0 + j] + mf[i + 1] * InM.mf[4 + j] + mf[i + 2] * InM.mf[8 + j] + mf[i + 3] * InM.mf[12 + j]);
			}
		}
		return res;
	}

	void matrix::operator*=(const matrix& InM) noexcept
	{
		*this = (InM * (*this));
	}

	vector3 matrix::operator*(const vector3& Point) const noexcept
	{
		float x = Point.x * mf[0] + Point.y * mf[4] + Point.z * mf[8] + mf[12];
		float y = Point.x * mf[1] + Point.y * mf[5] + Point.z * mf[9] + mf[13];
		float z = Point.x * mf[2] + Point.y * mf[6] + Point.z * mf[10] + mf[14];
		return vector3{ x, y, z };
	}

	void matrix::resetRotation() noexcept
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

	void matrix::setRotation(const vector3& rot) noexcept
	{
		quaternion(rot).toMatrix(*this);
	}

	void matrix::rotate(const vector3& rot) noexcept
	{
		rotateX(rot.x);
		rotateY(rot.y);
		rotateZ(rot.z);
	}

	void matrix::rotateCCW(const vector3& rot) noexcept
	{
		rotate(rot * -1.0f);
	}

	void matrix::rotateX(float fDegrees) noexcept
	{
		matrix tmp;
		tmp.setRotationX(fDegrees);
		*this *= tmp;
	}

	void matrix::rotateY(float fDegrees) noexcept
	{
		matrix tmp;
		tmp.setRotationY(fDegrees);
		*this *= tmp;
	}

	void matrix::rotateZ(float fDegrees) noexcept
	{
		matrix tmp;
		tmp.setRotationZ(fDegrees);
		*this *= tmp;
	}

	void matrix::setRotationX(float angle) noexcept
	{
		mf[5] = cosf(DEG_TO_RAD(angle));
		mf[6] = sinf(DEG_TO_RAD(angle));
		mf[9] = -sinf(DEG_TO_RAD(angle));
		mf[10] = cosf(DEG_TO_RAD(angle));
	}

	void matrix::setRotationY(float angle) noexcept
	{
		mf[0] = cosf(DEG_TO_RAD(angle));
		mf[2] = -sinf(DEG_TO_RAD(angle));
		mf[8] = sinf(DEG_TO_RAD(angle));
		mf[10] = cosf(DEG_TO_RAD(angle));
	}

	void matrix::setRotationZ(float angle) noexcept
	{
		mf[0] = cosf(DEG_TO_RAD(angle));
		mf[1] = sinf(DEG_TO_RAD(angle));
		mf[4] = -sinf(DEG_TO_RAD(angle));
		mf[5] = cosf(DEG_TO_RAD(angle));
	}

	void matrix::scale(float sx, float sy, float sz) noexcept
	{
		for (int x = 0; x < 4; x++)
		{
			mf[x] *= sx;
		}
		for (int x = 4; x < 8; x++)
		{
			mf[x] *= sy;
		}
		for (int x = 8; x < 12; x++)
		{
			mf[x] *= sz;
		}
	}

	void matrix::translate(const vector3& test)
	{
		for (int j = 0; j < 4; j++)
		{
			mf[12 + j] += test.x * mf[j] + test.y * mf[4 + j] + test.z * mf[8 + j];
		}
	}

	vector3 matrix::getTranslate() const noexcept
	{
		return vector3{ mf[12], mf[13], mf[14] };
	}

	void matrix::setTranslate(const vector3& b) noexcept
	{
		mf[12] = b.x;
		mf[13] = b.y;
		mf[14] = b.z;
	}

	void matrix::resetTranslate() noexcept
	{
		mf[12] = 0;
		mf[13] = 0;
		mf[14] = 0;
	}

	void matrix::toCCWRotationMatrix(float fDegrees, float x, float y, float z)
	{
		reset();

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

	matrix matrix::invertSimple()
	{
		matrix R;
		R.mf[0] = mf[0]; 		R.mf[1] = mf[4];		R.mf[2] = mf[8];	R.mf[3] = 0.0f;
		R.mf[4] = mf[1];		R.mf[5] = mf[5];		R.mf[6] = mf[9];	R.mf[7] = 0.0f;
		R.mf[8] = mf[2];		R.mf[9] = mf[6];		R.mf[10] = mf[10];	R.mf[11] = 0.0f;
		R.mf[12] = -(mf[12] * mf[0]) - (mf[13] * mf[1]) - (mf[14] * mf[2]);
		R.mf[13] = -(mf[12] * mf[4]) - (mf[13] * mf[5]) - (mf[14] * mf[6]);
		R.mf[14] = -(mf[12] * mf[8]) - (mf[13] * mf[9]) - (mf[14] * mf[10]);
		R.mf[15] = 1.0f;
		return R;
	}

	matrix matrix::invertRot()
	{
		matrix R;
		R.mf[0] = mf[0]; 		R.mf[1] = mf[4];		R.mf[2] = mf[8];	R.mf[3] = 0.0f;
		R.mf[4] = mf[1];		R.mf[5] = mf[5];		R.mf[6] = mf[9];	R.mf[7] = 0.0f;
		R.mf[8] = mf[2];		R.mf[9] = mf[6];		R.mf[10] = mf[10];	R.mf[11] = 0.0f;
		return R;
	}

	vector3 matrix::getRightVector() const noexcept
	{
		return vector3(mf[0], mf[1], mf[2]);
	}

	vector3 matrix::getForwardVector() const noexcept
	{
		return vector3(mf[4], mf[5], mf[6]);
	}

	vector3 matrix::getUpVector() const noexcept
	{
		return vector3(mf[8], mf[9], mf[10]);
	}
}
