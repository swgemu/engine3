/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef QUATERNION_H_
#define QUATERNION_H_

#include "Vector3.h"

#include "../io/Serializable.h"

namespace sys {
  namespace util {
  class Quaternion : public Serializable {
	protected:
		float w, x, y, z;

	public:
		static const Quaternion ZERO;
		static const Quaternion IDENTITY;

	public:
		/**
		* Creates a quaternion initialized to the quaternion identity.
		*/
		inline Quaternion() {
			w = 1.0f;
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;

			addSerializableVariables();
		}

		/**
		* Create a quaternion based on four scalar values.
		* float fx = vector x
		* float fy = vector y
		* float fz = vector z
		* float fw = scalar
		*/
		inline Quaternion(float fw, float fx, float fy, float fz) {
			w = fw;
			x = fx;
			y = fy;
			z = fz;

			addSerializableVariables();
		}

		/**
		* Create a quaternion based on a vector and an angle of direction.
		* \param v The vector to base rotation off of. Should be a UNIT vector.
		* \param angle The angle in radians.
		*/
		inline Quaternion(const Vector3& axis, float angle) {
			// Based on the formula q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k).
			float halfangle = angle * .5;
			float fsin = Math::sin(halfangle);

			w = Math::cos(halfangle);
			x = axis.x * fsin;
			y = axis.y * fsin;
			z = axis.z * fsin;

			addSerializableVariables();
		}

		//inline explicit Quaternion(const Matrix4& matrix) {
		//}

		virtual ~Quaternion() {

		}

		/**
		 * Returns the string representation of the vector in (x, y, z) format.
		 */

		void addSerializableVariables() {
			addSerializableVariable("w", &w);
			addSerializableVariable("x", &x);
			addSerializableVariable("y", &y);
			addSerializableVariable("z", &z);
		}

		inline Quaternion& operator = (const Quaternion& q) {
			w = q.w;
			x = q.x;
			y = q.y;
			z = q.z;

			return *this;
		}

		inline Quaternion operator + (const Quaternion& q) const {
			return Quaternion(w + q.w, x + q.x, y + q.y, z + q.z);
		}

		inline Quaternion operator - (const Quaternion& q) const {
			return Quaternion(w - q.w, x - q.x, y - q.y, z - q.z);
		}

		inline Quaternion operator * (const Quaternion& q) const {
			return Quaternion(
				w * q.w - x * q.x - y * q.y - z * q.z,
				w * q.x + x * q.w + y * q.z - z * q.y,
				w * q.y - x * q.z + y * q.w + z * q.x,
				w * q.z + x * q.y - y * q.x + z * q.w);
		}

		inline Quaternion operator * (const float scalar) const {
			return Quaternion(scalar * w, scalar * x, scalar * y, scalar * z);
		}

		inline bool operator == (const Quaternion& q) const {
			return (w == q.w && x == q.x && y == q.y && z == q.z);
		}

		inline bool operator != (const Quaternion& q) const {
			return (w != q.w || x != q.x || y != q.y || z != q.z);
		}

		inline float dotProduct(const Quaternion& q) const {
			return (w * q.w + x * q.x + y * q.y + z * q.z);
		}

		inline float length() const {
			return (w * w + x * x + y * y + z * z);
		}

		inline float normalize() {
			float len = length();

			float magnitude = Math::sqrt(len);
			w /= magnitude;
			x /= magnitude;
			y /= magnitude;
			z /= magnitude;

			return len;
		}

		/**
		* Rotates the quaternion on an axis of rotation n degrees.
		* \param axis The unit axis of rotation.
		* \param degrees How many degrees to rotate the quaternion.
		* \return Returns this quaternion, rotated.
		*/
		inline Quaternion& rotate(const Vector3& axis, float degrees) {
			Quaternion qrotate(axis, Math::deg2rad(degrees));
			*this = qrotate * *this;
			return *this;
		}

		/**
		* Converts this quaternion to a matrix4.
		* \return Matrix4 The matrix4 equivalent of this quaternion.
		*/
		//inline Matrix4 toMatrix() { }

		inline float getX() {
			return x;
		}

		inline float getY() {
			return y;
		}

		inline float getZ() {
			return z;
		}

		inline float getW() {
			return w;
		}

		inline bool isIdentity() {
			return (*this == IDENTITY);
		}
	};

  }
}

#endif /* QUATERNION_H_ */
