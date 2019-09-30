/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * Matrix4.cpp
 *
 *  Created on: 11/12/2010
 *      Author: victor
 */

#include "Matrix4.h"
#include "Matrix3.h"
#include "Quaternion.h"

const Vector3 Vector3::ZERO;

const Vector3 Vector3::UNIT_X(1, 0, 0);
const Vector3 Vector3::UNIT_Y(0, 1, 0);
const Vector3 Vector3::UNIT_Z(0, 0, 1);

const Quaternion Quaternion::ZERO(0, 0, 0, 0);
const Quaternion Quaternion::IDENTITY;

Matrix4::Matrix4(const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& d) : matrix{a, b, c, d} {
}

Matrix4::Matrix4(const Matrix4& mat) : Object(), matrix{mat.matrix[0], mat.matrix[1], mat.matrix[2], mat.matrix[3]} {
}

Matrix4& Matrix4::operator=(const Matrix4& mat) {
	if (this == &mat)
		return *this;

	for (int i = 0; i < 4; ++i)
		matrix[i] = mat.matrix[i];

	return *this;
}

void Matrix4::identity() {
	for (int i = 0; i < 4; ++i) {
		matrix[i].setZero();
		matrix[i][i] = 1.f;
	}
}

void Matrix4::setRotationMatrix(const Matrix3& rot) {
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j)
			matrix[i][j] = rot[i][j];

		matrix[i][3] = 0;
	}
}

Matrix4 Matrix4::operator*(const Matrix4& mat) const {
	return Matrix4(Vector4(matrix[0][0]*mat.matrix[0][0] + matrix[0][1]*mat.matrix[1][0] + matrix[0][2]*mat.matrix[2][0] + matrix[0][3]*mat.matrix[3][0],
			matrix[0][0]*mat.matrix[0][1] + matrix[0][1]*mat.matrix[1][1] + matrix[0][2]*mat.matrix[2][1] + matrix[0][3]*mat.matrix[3][1],
			matrix[0][0]*mat.matrix[0][2] + matrix[0][1]*mat.matrix[1][2] + matrix[0][2]*mat.matrix[2][2] + matrix[0][3]*mat.matrix[3][2],
			matrix[0][0]*mat.matrix[0][3] + matrix[0][1]*mat.matrix[1][3] + matrix[0][2]*mat.matrix[2][3] + matrix[0][3]*mat.matrix[3][3]),

			Vector4(matrix[1][0]*mat.matrix[0][0] + matrix[1][1]*mat.matrix[1][0] + matrix[1][2]*mat.matrix[2][0] + matrix[1][3]*mat.matrix[3][0],
					matrix[1][0]*mat.matrix[0][1] + matrix[1][1]*mat.matrix[1][1] + matrix[1][2]*mat.matrix[2][1] + matrix[1][3]*mat.matrix[3][1],
					matrix[1][0]*mat.matrix[0][2] + matrix[1][1]*mat.matrix[1][2] + matrix[1][2]*mat.matrix[2][2] + matrix[1][3]*mat.matrix[3][2],
					matrix[1][0]*mat.matrix[0][3] + matrix[1][1]*mat.matrix[1][3] + matrix[1][2]*mat.matrix[2][3] + matrix[1][3]*mat.matrix[3][3]),

					Vector4(matrix[2][0]*mat.matrix[0][0] + matrix[2][1]*mat.matrix[1][0] + matrix[2][2]*mat.matrix[2][0] + matrix[2][3]*mat.matrix[3][0],
							matrix[2][0]*mat.matrix[0][1] + matrix[2][1]*mat.matrix[1][1] + matrix[2][2]*mat.matrix[2][1] + matrix[2][3]*mat.matrix[3][1],
							matrix[2][0]*mat.matrix[0][2] + matrix[2][1]*mat.matrix[1][2] + matrix[2][2]*mat.matrix[2][2] + matrix[2][3]*mat.matrix[3][2],
							matrix[2][0]*mat.matrix[0][3] + matrix[2][1]*mat.matrix[1][3] + matrix[2][2]*mat.matrix[2][3] + matrix[2][3]*mat.matrix[3][3]),

							Vector4(matrix[3][0]*mat.matrix[0][0] + matrix[3][1]*mat.matrix[1][0] + matrix[3][2]*mat.matrix[2][0] + matrix[3][3]*mat.matrix[3][0],
									matrix[3][0]*mat.matrix[0][1] + matrix[3][1]*mat.matrix[1][1] + matrix[3][2]*mat.matrix[2][1] + matrix[3][3]*mat.matrix[3][1],
									matrix[3][0]*mat.matrix[0][2] + matrix[3][1]*mat.matrix[1][2] + matrix[3][2]*mat.matrix[2][2] + matrix[3][3]*mat.matrix[3][2],
									matrix[3][0]*mat.matrix[0][3] + matrix[3][1]*mat.matrix[1][3] + matrix[3][2]*mat.matrix[2][3] + matrix[3][3]*mat.matrix[3][3]));

}

Vector3 Matrix4::operator*(const Vector3& vec) const {
	const float x = vec.getX();
	const float y = vec.getY();
	const float z = vec.getZ();

	return Vector3(x * matrix[0][0] + y * matrix[1][0] + z * matrix[2][0] + matrix[3][0],
	               x * matrix[0][1] + y * matrix[1][1] + z * matrix[2][1] + matrix[3][1],
	               x * matrix[0][2] + y * matrix[1][2] + z * matrix[2][2] + matrix[3][2]);
}

Matrix4 Matrix4::inverse() const {
	//Written by Matthew Fisher  https://graphics.stanford.edu/~mdfisher/Code/Engine/Matrix4.cpp.html
	//
	// Inversion by Cramer's rule.  Code taken from an Intel publication
	//
	double Result[4][4];
	double tmp[12]; /* temp array for pairs */
	double src[16]; /* array of transpose source matrix */
	double det; /* determinant */
	/* transpose matrix */
	for (unsigned int i = 0; i < 4; i++) {
		src[i + 0] = matrix[i][0];
		src[i + 4] = matrix[i][1];
		src[i + 8] = matrix[i][2];
		src[i + 12] = matrix[i][3];
	}
	/* calculate pairs for first 8 elements (cofactors) */
	tmp[0] = src[10] * src[15];
	tmp[1] = src[11] * src[14];
	tmp[2] = src[9] * src[15];
	tmp[3] = src[11] * src[13];
	tmp[4] = src[9] * src[14];
	tmp[5] = src[10] * src[13];
	tmp[6] = src[8] * src[15];
	tmp[7] = src[11] * src[12];
	tmp[8] = src[8] * src[14];
	tmp[9] = src[10] * src[12];
	tmp[10] = src[8] * src[13];
	tmp[11] = src[9] * src[12];
	/* calculate first 8 elements (cofactors) */
	Result[0][0] = tmp[0] * src[5] + tmp[3] * src[6] + tmp[4] * src[7];
	Result[0][0] -= tmp[1] * src[5] + tmp[2] * src[6] + tmp[5] * src[7];
	Result[0][1] = tmp[1] * src[4] + tmp[6] * src[6] + tmp[9] * src[7];
	Result[0][1] -= tmp[0] * src[4] + tmp[7] * src[6] + tmp[8] * src[7];
	Result[0][2] = tmp[2] * src[4] + tmp[7] * src[5] + tmp[10] * src[7];
	Result[0][2] -= tmp[3] * src[4] + tmp[6] * src[5] + tmp[11] * src[7];
	Result[0][3] = tmp[5] * src[4] + tmp[8] * src[5] + tmp[11] * src[6];
	Result[0][3] -= tmp[4] * src[4] + tmp[9] * src[5] + tmp[10] * src[6];
	Result[1][0] = tmp[1] * src[1] + tmp[2] * src[2] + tmp[5] * src[3];
	Result[1][0] -= tmp[0] * src[1] + tmp[3] * src[2] + tmp[4] * src[3];
	Result[1][1] = tmp[0] * src[0] + tmp[7] * src[2] + tmp[8] * src[3];
	Result[1][1] -= tmp[1] * src[0] + tmp[6] * src[2] + tmp[9] * src[3];
	Result[1][2] = tmp[3] * src[0] + tmp[6] * src[1] + tmp[11] * src[3];
	Result[1][2] -= tmp[2] * src[0] + tmp[7] * src[1] + tmp[10] * src[3];
	Result[1][3] = tmp[4] * src[0] + tmp[9] * src[1] + tmp[10] * src[2];
	Result[1][3] -= tmp[5] * src[0] + tmp[8] * src[1] + tmp[11] * src[2];
	/* calculate pairs for second 8 elements (cofactors) */
	tmp[0] = src[2] * src[7];
	tmp[1] = src[3] * src[6];
	tmp[2] = src[1] * src[7];
	tmp[3] = src[3] * src[5];
	tmp[4] = src[1] * src[6];
	tmp[5] = src[2] * src[5];

	tmp[6] = src[0] * src[7];
	tmp[7] = src[3] * src[4];
	tmp[8] = src[0] * src[6];
	tmp[9] = src[2] * src[4];
	tmp[10] = src[0] * src[5];
	tmp[11] = src[1] * src[4];
	/* calculate second 8 elements (cofactors) */
	Result[2][0] = tmp[0] * src[13] + tmp[3] * src[14] + tmp[4] * src[15];
	Result[2][0] -= tmp[1] * src[13] + tmp[2] * src[14] + tmp[5] * src[15];
	Result[2][1] = tmp[1] * src[12] + tmp[6] * src[14] + tmp[9] * src[15];
	Result[2][1] -= tmp[0] * src[12] + tmp[7] * src[14] + tmp[8] * src[15];
	Result[2][2] = tmp[2] * src[12] + tmp[7] * src[13] + tmp[10] * src[15];
	Result[2][2] -= tmp[3] * src[12] + tmp[6] * src[13] + tmp[11] * src[15];
	Result[2][3] = tmp[5] * src[12] + tmp[8] * src[13] + tmp[11] * src[14];
	Result[2][3] -= tmp[4] * src[12] + tmp[9] * src[13] + tmp[10] * src[14];
	Result[3][0] = tmp[2] * src[10] + tmp[5] * src[11] + tmp[1] * src[9];
	Result[3][0] -= tmp[4] * src[11] + tmp[0] * src[9] + tmp[3] * src[10];
	Result[3][1] = tmp[8] * src[11] + tmp[0] * src[8] + tmp[7] * src[10];
	Result[3][1] -= tmp[6] * src[10] + tmp[9] * src[11] + tmp[1] * src[8];
	Result[3][2] = tmp[6] * src[9] + tmp[11] * src[11] + tmp[3] * src[8];
	Result[3][2] -= tmp[10] * src[11] + tmp[2] * src[8] + tmp[7] * src[9];
	Result[3][3] = tmp[10] * src[10] + tmp[4] * src[8] + tmp[9] * src[9];
	Result[3][3] -= tmp[8] * src[9] + tmp[11] * src[10] + tmp[5] * src[8];
	/* calculate determinant */
	det = src[0] * Result[0][0] + src[1] * Result[0][1] + src[2] * Result[0][2] + src[3] * Result[0][3];
	/* calculate matrix inverse */
	det = 1.0f / det;

	Matrix4 FloatResult;
	for (unsigned int i = 0; i < 4; i++) {
		for (unsigned int j = 0; j < 4; j++) {
			FloatResult[i][j] = float(Result[i][j] * det);
		}
	}
	return FloatResult;
}
