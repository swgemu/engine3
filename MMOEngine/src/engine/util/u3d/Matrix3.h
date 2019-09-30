/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * Matrix3.h
 *
 *  Created on: 11/12/2010
 *      Author: victor
 */

#pragma once

#include "Vector3.h"

namespace engine {
 namespace util {
  namespace u3d {

 class Matrix3 {
	 Vector3 matrix[3];
 public:
	 Matrix3() {

	 }

	 Matrix3(const Vector3& v1, const Vector3& v2, const Vector3& v3) : matrix{v1, v2, v3} {
	 }

	 Matrix3(const Matrix3& mat) : matrix{mat.matrix[0], mat.matrix[1], mat.matrix[2]} {
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

	 void transpose()  {
		 *this = Matrix3(Vector3(matrix[0][0], matrix[1][0], matrix[2][0]),
						Vector3(matrix[0][1], matrix[1][1], matrix[2][1]),
						Vector3(matrix[0][2], matrix[1][2], matrix[2][2]));
	 }

	 inline Vector3& operator [] (uint32 index) {
#ifdef VECTORS_OUT_OF_BOUNDS_CHECK
		 if (index > 2)
			 throw ArrayIndexOutOfBoundsException(index);
#endif
		 return matrix[index];
	 }

	 inline const Vector3& operator [] (uint32 index) const {
#ifdef VECTORS_OUT_OF_BOUNDS_CHECK
		 if (index > 2)
			 throw ArrayIndexOutOfBoundsException(index);
#endif
		 return matrix[index];
	 }


 };

  } // u3d
 } // util
} // engine

using namespace engine::util::u3d;

