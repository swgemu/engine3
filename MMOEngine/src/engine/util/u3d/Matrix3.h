/*
 * Matrix3.h
 *
 *  Created on: 11/12/2010
 *      Author: victor
 */

#ifndef MATRIX3_H_
#define MATRIX3_H_

#include "Vector3.h"

namespace engine {
 namespace util {
  namespace u3d {

 class Matrix3 {
	 Vector3 matrix[3];
 public:
	 Matrix3() {

	 }

	 Matrix3(const Vector3& v1, const Vector3& v2, const Vector3& v3) {
		 matrix[0] = v1;
		 matrix[1] = v2;
		 matrix[2] = v3;
	 }

	 Matrix3(const Matrix3& mat) {
		 for (int i = 0; i < 3; ++i)
			 matrix[i] = mat.matrix[i];
	 }

	 Matrix3& operator=(const Matrix3& mat) {
		 if (this == &mat)
			 return *this;

		 for (int i = 0; i < 3; ++i)
			 matrix[i] = mat.matrix[i];

		 return *this;
	 }

	 void scale(const Vector3& p) {
		 matrix[0][0] *= p.getX();	matrix[0][1] *= p.getX();	matrix[0][2] *= p.getX();
		 matrix[1][0] *= p.getY();	matrix[1][1] *= p.getY();	matrix[1][2] *= p.getY();
		 matrix[2][0] *= p.getZ();	matrix[2][1] *= p.getZ();	matrix[2][2] *= p.getZ();
	 }

	 void scale(float sx, float sy, float sz) {
		 matrix[0][0] *= sx;	matrix[0][1] *= sx;	matrix[0][2] *= sx;
		 matrix[1][0] *= sy;	matrix[1][1] *= sy;	matrix[1][2] *= sy;
		 matrix[2][0] *= sz;	matrix[2][1] *= sz;	matrix[2][2] *= sz;
	 }

	 inline Vector3& operator [] (uint32 index) {
		 if (index > 2)
			 throw ArrayIndexOutOfBoundsException(index);

		 return matrix[index];
	 }

	 inline const Vector3& operator [] (uint32 index) const {
		 if (index > 2)
			 throw ArrayIndexOutOfBoundsException(index);

		 return matrix[index];
	 }


 };

  } // u3d
 } // util
} // engine

using namespace engine::util::u3d;

#endif /* MATRIX3_H_ */
