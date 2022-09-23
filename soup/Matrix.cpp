#include "Matrix.hpp"

#include <cstring> // memcmp

#include "Quaternion.hpp"
#include "math.hpp"
#include "Vector3.hpp"

namespace soup
{
	Matrix::Matrix() noexcept
	{
		reset();
	}

	Matrix::Matrix(const Vector3& pos, const Vector3& rot) noexcept
	{
		setTranslate(pos);
		setRotation(rot);
		mf[15] = 1.0f;
	}

	Matrix Matrix::projection(float aspect_ratio, float fov, float z_near, float z_far)
	{
		float fov_rad = 1.0f / tanf(DEG_TO_RAD(fov) * 0.5f);

		Matrix m;
		m.mf[(4 * 0) + 0] = aspect_ratio * fov_rad;
		m.mf[(4 * 1) + 1] = fov_rad;
		m.mf[(4 * 2) + 2] = z_far / (z_far - z_near);
		m.mf[(4 * 3) + 2] = (-z_far * z_near) / (z_far - z_near);
		m.mf[(4 * 2) + 3] = 1.0f;
		m.mf[(4 * 3) + 3] = 0.0f;

		return m;
	}

	void Matrix::setPosRotXYZ(const Vector3& pos, const Vector3& rot) noexcept
	{
		setTranslate(pos);
		setRotationXYZ(rot);
		mf[15] = 1.0f;
	}

	void Matrix::reset() noexcept
	{
		mf[0] = 1.0f;	mf[1] = 0.0f;	mf[2] = 0.0f;	mf[3] = 0.0f;
		mf[4] = 0.0f;	mf[5] = 1.0f;	mf[6] = 0.0f;	mf[7] = 0.0f;
		mf[8] = 0.0f;	mf[9] = 0.0f;	mf[10] = 1.0f;	mf[11] = 0.0f;
		mf[12] = 0.0f;	mf[13] = 0.0f;	mf[14] = 0.0f;	mf[15] = 1.0f;
	}

	void Matrix::null() noexcept
	{
		mf[0] = 0.0f;	mf[1] = 0.0f;	mf[2] = 0.0f;	mf[3] = 0.0f;
		mf[4] = 0.0f;	mf[5] = 0.0f;	mf[6] = 0.0f;	mf[7] = 0.0f;
		mf[8] = 0.0f;	mf[9] = 0.0f;	mf[10] = 0.0f;	mf[11] = 0.0f;
		mf[12] = 0.0f;	mf[13] = 0.0f;	mf[14] = 0.0f;	mf[15] = 0.0f;
	}

	bool Matrix::operator==(const Matrix& b) const noexcept
	{
		return memcmp(mf, b.mf, sizeof(mf)) == 0;
	}

	bool Matrix::operator!=(const Matrix& b) const noexcept
	{
		return !operator==(b);
	}

	Matrix Matrix::operator*(const Matrix& InM) const noexcept
	{
		Matrix res;
		for (int i = 0; i < 16; i += 4)
		{
			for (int j = 0; j < 4; j++)
			{
				res.mf[i + j] = (mf[i + 0] * InM.mf[0 + j] + mf[i + 1] * InM.mf[4 + j] + mf[i + 2] * InM.mf[8 + j] + mf[i + 3] * InM.mf[12 + j]);
			}
		}
		return res;
	}

	void Matrix::operator*=(const Matrix& InM) noexcept
	{
		*this = (InM * (*this));
	}

	Vector3 Matrix::operator*(const Vector3& Point) const noexcept
	{
		float x = Point.x * mf[0] + Point.y * mf[4] + Point.z * mf[8] + mf[12];
		float y = Point.x * mf[1] + Point.y * mf[5] + Point.z * mf[9] + mf[13];
		float z = Point.x * mf[2] + Point.y * mf[6] + Point.z * mf[10] + mf[14];
		float w = Point.x * mf[3] + Point.y * mf[7] + Point.z * mf[11] + mf[15];
		return Vector3{ x / w, y / w, z / w };
	}

	void Matrix::resetRotation() noexcept
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

	void Matrix::setRotation(const Vector3& rot) noexcept
	{
		Quaternion::fromEuler(rot).toMatrix(*this);
	}

	void Matrix::setRotationXYZ(const Vector3& rot) noexcept
	{
		Quaternion::fromEulerXYZ(rot).toMatrix(*this);
	}

	void Matrix::rotate(const Vector3& rot) noexcept
	{
		rotateX(rot.x);
		rotateY(rot.y);
		rotateZ(rot.z);
	}

	void Matrix::rotateCCW(const Vector3& rot) noexcept
	{
		rotate(rot * -1.0f);
	}

	void Matrix::rotateX(float fDegrees) noexcept
	{
		Matrix tmp;
		tmp.setRotationX(fDegrees);
		*this *= tmp;
	}

	void Matrix::rotateY(float fDegrees) noexcept
	{
		Matrix tmp;
		tmp.setRotationY(fDegrees);
		*this *= tmp;
	}

	void Matrix::rotateZ(float fDegrees) noexcept
	{
		Matrix tmp;
		tmp.setRotationZ(fDegrees);
		*this *= tmp;
	}

	void Matrix::setRotationX(float angle) noexcept
	{
		mf[5] = cosf(DEG_TO_RAD(angle));
		mf[6] = sinf(DEG_TO_RAD(angle));
		mf[9] = -sinf(DEG_TO_RAD(angle));
		mf[10] = cosf(DEG_TO_RAD(angle));
	}

	void Matrix::setRotationY(float angle) noexcept
	{
		mf[0] = cosf(DEG_TO_RAD(angle));
		mf[2] = -sinf(DEG_TO_RAD(angle));
		mf[8] = sinf(DEG_TO_RAD(angle));
		mf[10] = cosf(DEG_TO_RAD(angle));
	}

	void Matrix::setRotationZ(float angle) noexcept
	{
		mf[0] = cosf(DEG_TO_RAD(angle));
		mf[1] = sinf(DEG_TO_RAD(angle));
		mf[4] = -sinf(DEG_TO_RAD(angle));
		mf[5] = cosf(DEG_TO_RAD(angle));
	}

	void Matrix::scale(float f) noexcept
	{
		for (int x = 0; x != 12; ++x)
		{
			mf[x] *= f;
		}
	}

	void Matrix::scale(float sx, float sy, float sz) noexcept
	{
		for (int x = 0; x != 4; ++x)
		{
			mf[x] *= sx;
		}
		for (int x = 4; x != 8; ++x)
		{
			mf[x] *= sy;
		}
		for (int x = 8; x != 12; ++x)
		{
			mf[x] *= sz;
		}
	}

	void Matrix::translate(const Vector3& test)
	{
		for (int j = 0; j < 4; j++)
		{
			mf[12 + j] += test.x * mf[j] + test.y * mf[4 + j] + test.z * mf[8 + j];
		}
	}

	Vector3 Matrix::getTranslate() const noexcept
	{
		return Vector3{ mf[12], mf[13], mf[14] };
	}

	void Matrix::setTranslate(const Vector3& b) noexcept
	{
		mf[12] = b.x;
		mf[13] = b.y;
		mf[14] = b.z;
	}

	void Matrix::resetTranslate() noexcept
	{
		mf[12] = 0;
		mf[13] = 0;
		mf[14] = 0;
	}

	void Matrix::toCCWRotationMatrix(float fDegrees, float x, float y, float z)
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

	Matrix Matrix::invert() const noexcept
	{
		Matrix inv;

		inv.mf[0] = mf[5] * mf[10] * mf[15] -
			mf[5] * mf[11] * mf[14] -
			mf[9] * mf[6] * mf[15] +
			mf[9] * mf[7] * mf[14] +
			mf[13] * mf[6] * mf[11] -
			mf[13] * mf[7] * mf[10];

		inv.mf[4] = -mf[4] * mf[10] * mf[15] +
			mf[4] * mf[11] * mf[14] +
			mf[8] * mf[6] * mf[15] -
			mf[8] * mf[7] * mf[14] -
			mf[12] * mf[6] * mf[11] +
			mf[12] * mf[7] * mf[10];

		inv.mf[8] = mf[4] * mf[9] * mf[15] -
			mf[4] * mf[11] * mf[13] -
			mf[8] * mf[5] * mf[15] +
			mf[8] * mf[7] * mf[13] +
			mf[12] * mf[5] * mf[11] -
			mf[12] * mf[7] * mf[9];

		inv.mf[12] = -mf[4] * mf[9] * mf[14] +
			mf[4] * mf[10] * mf[13] +
			mf[8] * mf[5] * mf[14] -
			mf[8] * mf[6] * mf[13] -
			mf[12] * mf[5] * mf[10] +
			mf[12] * mf[6] * mf[9];

		inv.mf[1] = -mf[1] * mf[10] * mf[15] +
			mf[1] * mf[11] * mf[14] +
			mf[9] * mf[2] * mf[15] -
			mf[9] * mf[3] * mf[14] -
			mf[13] * mf[2] * mf[11] +
			mf[13] * mf[3] * mf[10];

		inv.mf[5] = mf[0] * mf[10] * mf[15] -
			mf[0] * mf[11] * mf[14] -
			mf[8] * mf[2] * mf[15] +
			mf[8] * mf[3] * mf[14] +
			mf[12] * mf[2] * mf[11] -
			mf[12] * mf[3] * mf[10];

		inv.mf[9] = -mf[0] * mf[9] * mf[15] +
			mf[0] * mf[11] * mf[13] +
			mf[8] * mf[1] * mf[15] -
			mf[8] * mf[3] * mf[13] -
			mf[12] * mf[1] * mf[11] +
			mf[12] * mf[3] * mf[9];

		inv.mf[13] = mf[0] * mf[9] * mf[14] -
			mf[0] * mf[10] * mf[13] -
			mf[8] * mf[1] * mf[14] +
			mf[8] * mf[2] * mf[13] +
			mf[12] * mf[1] * mf[10] -
			mf[12] * mf[2] * mf[9];

		inv.mf[2] = mf[1] * mf[6] * mf[15] -
			mf[1] * mf[7] * mf[14] -
			mf[5] * mf[2] * mf[15] +
			mf[5] * mf[3] * mf[14] +
			mf[13] * mf[2] * mf[7] -
			mf[13] * mf[3] * mf[6];

		inv.mf[6] = -mf[0] * mf[6] * mf[15] +
			mf[0] * mf[7] * mf[14] +
			mf[4] * mf[2] * mf[15] -
			mf[4] * mf[3] * mf[14] -
			mf[12] * mf[2] * mf[7] +
			mf[12] * mf[3] * mf[6];

		inv.mf[10] = mf[0] * mf[5] * mf[15] -
			mf[0] * mf[7] * mf[13] -
			mf[4] * mf[1] * mf[15] +
			mf[4] * mf[3] * mf[13] +
			mf[12] * mf[1] * mf[7] -
			mf[12] * mf[3] * mf[5];

		inv.mf[14] = -mf[0] * mf[5] * mf[14] +
			mf[0] * mf[6] * mf[13] +
			mf[4] * mf[1] * mf[14] -
			mf[4] * mf[2] * mf[13] -
			mf[12] * mf[1] * mf[6] +
			mf[12] * mf[2] * mf[5];

		inv.mf[3] = -mf[1] * mf[6] * mf[11] +
			mf[1] * mf[7] * mf[10] +
			mf[5] * mf[2] * mf[11] -
			mf[5] * mf[3] * mf[10] -
			mf[9] * mf[2] * mf[7] +
			mf[9] * mf[3] * mf[6];

		inv.mf[7] = mf[0] * mf[6] * mf[11] -
			mf[0] * mf[7] * mf[10] -
			mf[4] * mf[2] * mf[11] +
			mf[4] * mf[3] * mf[10] +
			mf[8] * mf[2] * mf[7] -
			mf[8] * mf[3] * mf[6];

		inv.mf[11] = -mf[0] * mf[5] * mf[11] +
			mf[0] * mf[7] * mf[9] +
			mf[4] * mf[1] * mf[11] -
			mf[4] * mf[3] * mf[9] -
			mf[8] * mf[1] * mf[7] +
			mf[8] * mf[3] * mf[5];

		inv.mf[15] = mf[0] * mf[5] * mf[10] -
			mf[0] * mf[6] * mf[9] -
			mf[4] * mf[1] * mf[10] +
			mf[4] * mf[2] * mf[9] +
			mf[8] * mf[1] * mf[6] -
			mf[8] * mf[2] * mf[5];

		float det = mf[0] * inv.mf[0] + mf[1] * inv.mf[4] + mf[2] * inv.mf[8] + mf[3] * inv.mf[12];

		if (det != 0.0f)
		{
			det = 1.0f / det;

			for (int i = 0; i != 16; ++i)
			{
				inv.mf[i] *= det;
			}
		}

		return inv;
	}

	Matrix Matrix::invertSimple() const noexcept
	{
		Matrix R;
		R.mf[0] = mf[0]; 		R.mf[1] = mf[4];		R.mf[2] = mf[8];	R.mf[3] = 0.0f;
		R.mf[4] = mf[1];		R.mf[5] = mf[5];		R.mf[6] = mf[9];	R.mf[7] = 0.0f;
		R.mf[8] = mf[2];		R.mf[9] = mf[6];		R.mf[10] = mf[10];	R.mf[11] = 0.0f;
		R.mf[12] = -(mf[12] * mf[0]) - (mf[13] * mf[1]) - (mf[14] * mf[2]);
		R.mf[13] = -(mf[12] * mf[4]) - (mf[13] * mf[5]) - (mf[14] * mf[6]);
		R.mf[14] = -(mf[12] * mf[8]) - (mf[13] * mf[9]) - (mf[14] * mf[10]);
		R.mf[15] = 1.0f;
		return R;
	}

	Matrix Matrix::invertRot() const noexcept
	{
		Matrix R;
		R.mf[0] = mf[0]; 		R.mf[1] = mf[4];		R.mf[2] = mf[8];	R.mf[3] = 0.0f;
		R.mf[4] = mf[1];		R.mf[5] = mf[5];		R.mf[6] = mf[9];	R.mf[7] = 0.0f;
		R.mf[8] = mf[2];		R.mf[9] = mf[6];		R.mf[10] = mf[10];	R.mf[11] = 0.0f;
		return R;
	}

	Vector3 Matrix::getRightVector() const noexcept
	{
		return Vector3(mf[0], mf[1], mf[2]);
	}

	Vector3 Matrix::getForwardVector() const noexcept
	{
		return Vector3(mf[4], mf[5], mf[6]);
	}

	Vector3 Matrix::getUpVector() const noexcept
	{
		return Vector3(mf[8], mf[9], mf[10]);
	}
}
