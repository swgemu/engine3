/*
 * Matrix4.cpp
 *
 *  Created on: 11/12/2010
 *      Author: victor
 */

#include "Matrix4.h"
#include "Matrix3.h"

const Vector3 Vector3::ZERO;

const Vector3 Vector3::UNIT_X(1, 0, 0);
const Vector3 Vector3::UNIT_Y(0, 1, 0);
const Vector3 Vector3::UNIT_Z(0, 0, 1);

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
	float x = vec.getX();
	float y = vec.getY();
	float z = vec.getZ();

	return Vector3(x * matrix[0][0] + y * matrix[1][0] + z * matrix[2][0] + matrix[3][0],
	               x * matrix[0][1] + y * matrix[1][1] + z * matrix[2][1] + matrix[3][1],
	               x * matrix[0][2] + y * matrix[1][2] + z * matrix[2][2] + matrix[3][2]);
}