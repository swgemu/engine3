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

void Matrix4::setRotationMatrix(const Matrix3& rot) {
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j)
			matrix[i][j] = rot[i][j];

		matrix[i][3] = 0;
	}
}
