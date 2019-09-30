/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * Matrix4.h
 *
 *  Created on: 11/12/2010
 *      Author: victor
 */

#pragma once

#include "Vector4.h"
#include "system/lang/Object.h"

namespace engine {
 namespace util {
  namespace u3d {

 class Matrix3;
 class Vector3;

 class Matrix4 : public Object {
	 Vector4 matrix[4];

 public:
	 Matrix4() {
		 identity();
	 }

	 Matrix4(const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& d);

	 Matrix4(const Matrix4& mat);

	 Matrix4& operator=(const Matrix4& op);

	 void identity();

	 inline Vector4& operator [] (uint32 index) {
#ifdef VECTORS_OUT_OF_BOUNDS_CHECK
		 if (index > 3)
			 throw ArrayIndexOutOfBoundsException(index);
#endif
		 return matrix[index];
	 }

	 inline const Vector4& operator [] (uint32 index) const {
#ifdef VECTORS_OUT_OF_BOUNDS_CHECK
		 if (index > 3)
			 throw ArrayIndexOutOfBoundsException(index);
#endif
		 return matrix[index];
	 }

	 inline void setTranslation(float x, float y, float z) {
		 matrix[3][0] = x;
		 matrix[3][1] = y;
		 matrix[3][2] = z;
		 matrix[3][3] = 1.f;
	 }

	 inline void swapLtoR() {
		 matrix[0][2] = -matrix[0][2];
		 matrix[1][2] = -matrix[1][2];
		 matrix[2][0] = -matrix[2][0];
		 matrix[2][1] = -matrix[2][1];
		 matrix[3][2] = -matrix[3][2];
	 }

	 void transpose()  {
		 *this = Matrix4(Vector4(matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0]),
						Vector4(matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1]),
						Vector4(matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2]),
						Vector4(matrix[0][3], matrix[1][3], matrix[2][3], matrix[3][3]));
	 }

	 Matrix4 inverse() const;

	 void setRotationMatrix(const Matrix3& rot);

	 Matrix4 operator*(const Matrix4& mat) const;
	 Vector3 operator*(const Vector3& mat) const;

 };

  } // u3d
 } // util
} // engine

using namespace engine::util::u3d;

