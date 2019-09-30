/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#pragma once

#include "system/io/Serializable.h"
#include "system/lang/Math.h"
#include "system/lang/DivisionByZeroException.h"

#include "Matrix4.h"

namespace engine {
  namespace util {
  	namespace u3d {

	class Vector3 {
	protected:
		float values[3];

	public:
		static const Vector3 ZERO;

		//Axis Representations
		static const Vector3 UNIT_X;
		static const Vector3 UNIT_Y;
		static const Vector3 UNIT_Z;

	public:
		inline Vector3() noexcept {
			values[0] = 0;
			values[1] = 0;
			values[2] = 0;
		}

		Vector3(const Vector3& vec) = default;

		inline Vector3(const float fx, const float fy, const float fz) noexcept {
			values[0] = fx;
			values[1] = fy;
			values[2] = fz;
		}

		/**
		 * Converts a 3 dimensional float array into a Vector3.
		 */
		inline explicit Vector3(const float coord[3]) noexcept {
			values[0] = coord[0];
			values[1] = coord[1];
			values[2] = coord[2];
		}

		/**
		 * Converts a 3 dimensional int array into a Vector3.
		 */
		inline explicit Vector3(const int coord[3]) noexcept {
			values[0] = (float) coord[0];
			values[1] = (float) coord[1];
			values[2] = (float) coord[2];
		}

		/**
		 * Converts a float scalar to a Vector3.
		 */
		inline explicit Vector3(const float scalar) noexcept {
			values[0] = scalar;
			values[1] = scalar;
			values[2] = scalar;
		}

	public:

		bool toBinaryStream(ObjectOutputStream* stream) {
			stream->writeFloat(values[0]);
			stream->writeFloat(values[2]);
			stream->writeFloat(values[1]);

			return true;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			values[0] = stream->readFloat();
			values[2] = stream->readFloat();
			values[1] = stream->readFloat();

			return true;
		}

		/**
		 * Returns the exact length of the vector. Should be used sparingly as it
		 * uses much CPU power. Use squaredLength for comparing lengths.
		 */
		inline float length() const {
			return Math::sqrt(values[0] * values[0] + values[1] * values[1] + values[2] * values[2]);
		}

		/**
		 * Returns the length before being squared. Good for comparing lengths.
		 */
		inline float squaredLength() const {
			return (values[0] * values[0] + values[1] * values[1] + values[2] * values[2]);
		}

		/**
		 * Returns the real distance to another vector. Should not be used for comparisons.
		 * See squaredDistanceTo(.
		 * \param v The vector to get the distance to.
		 * \return Returns the float representation of the distance between the two vectors.
		 */
		inline float distanceTo(const Vector3& v) const {
			return (*this - v).length();
		}

		/**
		 * Good for comparisons of distance. Saves the cpu usage needed for sqrt.
		 * \param v The vector to get the distance to.
		 * \return Returns the float representation of the distance between two points prior to application of a square root.
		 */
		inline float squaredDistanceTo(const Vector3& v) const {
			return (*this - v).squaredLength();
		}

		/**
		 * This method returns the midpoint between this vector and another vector.
		 * \param v The vector to find a midpoint in conjunction with.
		 * \return Returns a vector half way in between this vector and the passed in vector.
		 */
		inline Vector3 midPoint(const Vector3& v) const {
			return Vector3((values[0] + v.values[0]) * 0.5f, (values[1] + v.values[1]) * 0.5f, (values[2] + v.values[2]) * 0.5f);
		}

		/**
		 * This method is used to calculate the angle between two vectors. If both
		 * are unit vectors then the product is the cosine of the angle; otherwise,
		 * the dot product must be divided by the product of the length
		 */
		inline float dotProduct(const Vector3& v) const {
			return (values[0] * v.values[0] + values[1] * v.values[1] + values[2] * v.values[2]);
		}

		inline float product() const {
			return dotProduct(*this);
		}

		/**
		 * The cross product returns the vector which is perpendicular to this vector
		 * and the vector passed in. For example, if you have two vectors in the same
		 * geometric plane, then the vector would be on the plane perpendicular to that
		 * plane, and the vector perpendicular to the two vectors. Imagine a 3D o
		 */
		inline Vector3 crossProduct(const Vector3& v) const {
			return Vector3(values[1] * v.values[2] - values[2] * v.values[1], values[2] * v.values[0] - values[0] * v.values[2], values[0] * v.values[1] - values[1] * v.values[0]);
		}

		/**
		 * Returns the string representation of the vector in (x, y, z) format.
		 */
		inline String toString() const {
			StringBuffer sb;
			sb <<"(x:" << values[0] << ", y:" << values[1] << ", z:" << values[2] << ")";
			return sb.toString();
		}

		inline float operator [] (uint32 index) const {
#ifdef VECTORS_OUT_OF_BOUNDS_CHECK
			if (index > 2)
				throw ArrayIndexOutOfBoundsException(index);
#endif
			return values[index];
		}

		inline float& operator [] (uint32 index) {
#ifdef VECTORS_OUT_OF_BOUNDS_CHECK
			if (index > 2)
				throw ArrayIndexOutOfBoundsException(index);
#endif
			return values[index];
		}

		inline float* toFloatArray() {
			return values;
		}

		inline const float* toFloatArray() const {
			return values;
		}

		Vector3& operator = (const Vector3& v) = default;

		inline Vector3& operator = (const float scalar) {
			values[0] = scalar;
			values[1] = scalar;
			values[2] = scalar;

			return *this;
		}

		inline Vector3 operator + (const Vector3& v) const {
			return Vector3(values[0] + v.values[0], values[1] + v.values[1], values[2] + v.values[2]);
		}

		inline Vector3 operator - (const Vector3& v) const {
			return Vector3(values[0] - v.values[0], values[1] - v.values[1], values[2] - v.values[2]);
		}

		inline Vector3 operator * (const Vector3& v) const {
			return Vector3(values[0] * v.values[0], values[1] * v.values[1], values[2] * v.values[2]);
		}

		Vector3 operator*(const Matrix4& mat) const {
			return Vector3(values[0] * mat[0][0] + values[1] * mat[1][0] + values[2] * mat[2][0] + mat[3][0],
					values[0] * mat[0][1] + values[1] * mat[1][1] + values[2] * mat[2][1] + mat[3][1],
					values[0] * mat[0][2] + values[1] * mat[1][2] + values[2] * mat[2][2] + mat[3][2]);
		}

		inline Vector3 operator / (const Vector3& v) const {
			return Vector3(values[0] / v.values[0], values[1] / v.values[1], values[2] / v.values[2]);
		}

		inline Vector3& operator += (const Vector3& v) {
			values[0] += v.values[0];
			values[1] += v.values[1];
			values[2] += v.values[2];

			return *this;
		}

		inline Vector3& operator -= (const Vector3& v) {
			values[0] -= v.values[0];
			values[1] -= v.values[1];
			values[2] -= v.values[2];

			return *this;
		}

		inline Vector3& operator *= (const Vector3& v) {
			values[0] *= v.values[0];
			values[1] *= v.values[1];
			values[2] *= v.values[2];

			return *this;
		}

		inline Vector3& operator /= (const Vector3& v) {
			values[0] /= v.values[0];
			values[1] /= v.values[1];
			values[2] /= v.values[2];

			return *this;
		}

		inline friend Vector3 operator + (const Vector3& v, const float scalar) {
			return Vector3(v.values[0] + scalar, v.values[1] + scalar, v.values[2] + scalar);
		}

		inline friend Vector3 operator + (const float scalar, const Vector3& v) {
			return Vector3(scalar + v.values[0], scalar + v.values[1], scalar + v.values[2]);
		}

		inline friend Vector3 operator - (const Vector3& v, const float scalar) {
			return Vector3(v.values[0] - scalar, v.values[1] - scalar, v.values[2] - scalar);
		}

		inline friend Vector3 operator - (const float scalar, const Vector3& v) {
			return Vector3(scalar - v.values[0], scalar - v.values[1], scalar - v.values[2]);
		}

		inline friend Vector3 operator * (const Vector3& v, const float scalar) {
			return Vector3(v.values[0] * scalar, v.values[1] * scalar, v.values[2] * scalar);
		}

		inline friend Vector3 operator * (const float scalar, const Vector3& v) {
			return Vector3(scalar * v.values[0], scalar * v.values[1], scalar * v.values[2]);
		}

		inline friend Vector3 operator / (const Vector3& v, const float scalar) {
			if (scalar == 0.0f)
				throw DivisionByZeroException();

			return Vector3(v.values[0] / scalar, v.values[1] / scalar, v.values[2] / scalar);
		}

		inline friend Vector3 operator / (const float scalar, const Vector3& v) {
			return Vector3(scalar / v.values[0], scalar / v.values[1], scalar / v.values[2]);
		}

		//Boolean operators
		inline bool operator == (const Vector3& v) const {
			return values[0] == v.values[0] && values[1] == v.values[1] && values[2] == v.values[2];
		}

		inline bool operator != (const Vector3& v) const {
			return values[0] != v.values[0] || values[1] != v.values[1] || values[2] != v.values[2];
		}

		inline bool operator < (const Vector3& v) const {
			if (values[0] < v.values[0] && values[1] < v.values[1] && values[2] < v.values[2])
				return true;

			return false;
		}

		inline bool operator > (const Vector3& v) const {
			if (values[0] > v.values[0] && values[1] > v.values[1] && values[2] > v.values[2])
				return true;

			return false;
		}

		Vector3 getMin(const Vector3& vec) const {
			return Vector3(Math::min(values[0], vec.values[0]),
					Math::min(values[1], vec.values[1]),
					Math::min(values[2], vec.values[2]));
		}

		inline Vector3 getMax(const Vector3& vec) const {
			return Vector3(Math::max(values[0], vec.values[0]),
					Math::max(values[1], vec.values[1]),
					Math::max(values[2], vec.values[2]));
		}

		inline float normalize() {
			float magnitude = length();

			values[0] /= magnitude;
			values[1] /= magnitude;
			values[2] /= magnitude;

			return magnitude;
		}

		//Getters
		inline float getX() const {
			return values[0];
		}

		inline float getY() const {
			return values[1];
		}

		inline float getZ() const {
			return values[2];
		}

		//Setters
		inline void setX(float xv) {
			values[0] = xv;
		}

		inline void setY(float yv) {
			values[1] = yv;
		}

		inline void setZ(float zv) {
			values[2] = zv;
		}

		inline void set(float x, float z, float y) {
			this->values[0] = x;
			this->values[1] = y;
			this->values[2] = z;
		}

		friend class Quaternion;
	};

	class SerializableVector3 : public Vector3, public Variable {
	public:
		SerializableVector3()  {
		}

		SerializableVector3(const SerializableVector3& v) : Vector3(v), Variable() {
		}

		SerializableVector3(const Vector3& vec) : Vector3(vec) {
		}

		SerializableVector3& operator=(const Vector3& v) {
			Vector3::operator=(v);

			return *this;
		}

		bool toBinaryStream(ObjectOutputStream* stream) {
			return Vector3::toBinaryStream(stream);
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			return Vector3::parseFromBinaryStream(stream);
		}
	};

  	} // u3d
  } // util
} // engine

using namespace engine::util::u3d;

