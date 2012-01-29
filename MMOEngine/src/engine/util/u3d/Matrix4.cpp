/*
 * Matrix4.cpp
 *
 *  Created on: 11/12/2010
 *      Author: victor
 */

#include "Matrix4.h"
#include "Matrix3.h"
#include "Vector3.h"
#include "Vector4.h"

Matrix4::Matrix4(const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& d) {
	matrix[0] = a;
	matrix[1] = b;
	matrix[2] = c;
	matrix[3] = d;
}

Matrix4::Matrix4(const Matrix4& mat) : Object() {
	for (int i = 0; i < 4; ++i)
		matrix[i] = mat.matrix[i];
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
