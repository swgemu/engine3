/*
 * Matrix4.h
 *
 *  Created on: 11/12/2010
 *      Author: victor
 */

#ifndef MATRIX4_H_
#define MATRIX4_H_

#include "Vector4.h"

namespace engine {
 namespace util {

 class Matrix3;

 class Matrix4 {
	 Vector4 matrix[4];

 public:
	 Matrix4() {
		 identity();
	 }

	 Matrix4(const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& d) {
		 matrix[0] = a;
		 matrix[1] = b;
		 matrix[2] = c;
		 matrix[3] = d;
	 }

	 Matrix4(const Matrix4& mat) {
		 for (int i = 0; i < 4; ++i)
			 matrix[i] = mat.matrix[i];
	 }

	 void identity() {
		 for (int i = 0; i < 4; ++i) {
			 matrix[i].setZero();
			 matrix[i][i] = 1.f;
		 }
	 }

	 inline Vector4& operator [] (uint32 index) {
		 if (index > 3)
			 throw ArrayIndexOutOfBoundsException(index);

		 return matrix[index];
	 }

	 inline Vector4 operator [] (uint32 index) const {
		 if (index > 3)
			 throw ArrayIndexOutOfBoundsException(index);

		 return matrix[index];
	 }

	 inline void setTranslation(float x, float y, float z) {
		 matrix[3][0] = x;
		 matrix[3][1] = y;
		 matrix[3][2] = z;
		 matrix[3][3] = 1.f;
	 }

	 void setRotationMatrix(const Matrix3& rot);

	 Matrix4 operator*(const Matrix4& mat) const {
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

 };

 }
}

using namespace engine::util;

#endif /* MATRIX4_H_ */
