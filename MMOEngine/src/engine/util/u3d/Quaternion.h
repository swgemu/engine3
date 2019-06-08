/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef QUATERNION_H_
#define QUATERNION_H_

#include "system/io/Serializable.h"
#include "system/lang/String.h"

#include "Vector3.h"
#include "Matrix3.h"

namespace engine {
namespace util {
namespace u3d {

class Quaternion : public Variable {
protected:
	float w, x, y, z;

public:
	static const Quaternion ZERO;
	static const Quaternion IDENTITY;

public:
	/**
	 * Creates a quaternion initialized to the quaternion identity.
	 */
	Quaternion() {
		w = 1.0f;
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	Quaternion(const Quaternion& qt) : Variable() {
		w = qt.w;
		x = qt.x;
		y = qt.y;
		z = qt.z;
	}

	/**
	 * Create a quaternion based on four scalar values.
	 * float fx = vector x
	 * float fy = vector y
	 * float fz = vector z
	 * float fw = scalar
	 */
	Quaternion(float fw, float fx, float fy, float fz) {
		w = fw;
		x = fx;
		y = fy;
		z = fz;
	}


	/**
	 * Create a quaternion based on a vector and an angle of direction.
	 * \param v The vector to base rotation off of. Should be a UNIT vector.
	 * \param angle The angle in radians.
	 */
	Quaternion(const Vector3& axis, float angle) {
		// Based on the formula q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k).
		float halfangle = angle * .5;
		float fsin = Math::sin(halfangle);

		w = Math::cos(halfangle);
		x = axis.getX() * fsin;
		y = axis.getY() * fsin;
		z = axis.getZ() * fsin;
	}

	//inline explicit Quaternion(const Matrix4& matrix) {
	//}

	virtual ~Quaternion() {

	}

	/**
	 * Returns the string representation of the vector in (x, y, z) format.
	 */
	/*
		void addSerializableVariables() {
			addSerializableVariable("w", &w);
			addSerializableVariable("x", &x);
			addSerializableVariable("y", &y);
			addSerializableVariable("z", &z);
		}
	 */

	bool readObjectMember(ObjectInputStream* stream, const String& name) {
		if (name == "x") {
			TypeInfo<float>::parseFromBinaryStream(&x, stream);

			return true;
		} else if (name == "w") {
			TypeInfo<float>::parseFromBinaryStream(&w, stream);

			return true;
		} else if (name == "y") {
			TypeInfo<float>::parseFromBinaryStream(&y, stream);

			return true;
		} else if (name == "z") {
			TypeInfo<float>::parseFromBinaryStream(&z, stream);

			return true;
		}

		return false;
	}

	int writeObjectMembers(ObjectOutputStream* stream) {
		String _name;
		int _offset;
		uint32 _totalSize;

		_name = "z";
		_name.toBinaryStream(stream);
		_offset = stream->getOffset();
		stream->writeInt(0);
		TypeInfo<float>::toBinaryStream(&z, stream);
		_totalSize = (uint32) (stream->getOffset() - (_offset + 4));
		stream->writeInt(_offset, _totalSize);

		_name = "y";
		_name.toBinaryStream(stream);
		_offset = stream->getOffset();
		stream->writeInt(0);
		TypeInfo<float>::toBinaryStream(&y, stream);
		_totalSize = (uint32) (stream->getOffset() - (_offset + 4));
		stream->writeInt(_offset, _totalSize);

		_name = "x";
		_name.toBinaryStream(stream);
		_offset = stream->getOffset();
		stream->writeInt(0);
		TypeInfo<float >::toBinaryStream(&x, stream);
		_totalSize = (uint32) (stream->getOffset() - (_offset + 4));
		stream->writeInt(_offset, _totalSize);

		_name = "w";
		_name.toBinaryStream(stream);
		_offset = stream->getOffset();
		stream->writeInt(0);
		TypeInfo<float>::toBinaryStream(&w, stream);
		_totalSize = (uint32) (stream->getOffset() - (_offset + 4));
		stream->writeInt(_offset, _totalSize);

		String emptyName; // making it serialize the same way as Serializable so bas doesnt have to update all the objects

		_name = "_className";
		_name.toBinaryStream(stream);
		_offset = stream->getOffset();
		stream->writeInt(0);
		TypeInfo<String>::toBinaryStream(&emptyName, stream);
		_totalSize = (uint32) (stream->getOffset() - (_offset + 4));
		stream->writeInt(_offset, _totalSize);

		return 5;
	}

	bool toBinaryStream(ObjectOutputStream* stream) {
		int _currentOffset = stream->getOffset();
		stream->writeShort(0);
		int _varCount = writeObjectMembers(stream);
		stream->writeShort(_currentOffset, _varCount);

		return true;
	}

	bool parseFromBinaryStream(ObjectInputStream* stream) {
		uint16 _varCount = stream->readShort();

		for (int i = 0; i < _varCount; ++i) {
			String _name;
			_name.parseFromBinaryStream(stream);

			uint32 _varSize = stream->readInt();

			int _currentOffset = stream->getOffset();

			if(readObjectMember(stream, _name)) {
			}

			stream->setOffset(_currentOffset + _varSize);
		}

		return true;
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
				w * q.y + y * q.w + z * q.x - x * q.z,
				w * q.z + z * q.w + x * q.y - y * q.x);
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

	inline float lengthSquared() const {
		return (w * w + x * x + y * y + z * z);
	}

	inline float length() const {
		return Math::sqrt(lengthSquared());
	}

	inline float normalize() {
		float magnitude = length();

		w /= magnitude;
		x /= magnitude;
		y /= magnitude;
		z /= magnitude;

		return magnitude;
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

	Matrix3 toMatrix3() const {
		/*float len = length();

			if (len > 0.9999 && len < 1.0001)
				throw Exception("quaternion is not normalized");*/

		return Matrix3(Vector3(1 - 2 * (y * y +  z * z), 2 * (x * y - w * z), 2 * (x * z + w * y)),
				Vector3(2 * (x * y + w * z), 1 - 2 * (x * x + z * z), 2 * (y * z - w * x)),
				Vector3(2 * (x * z - w * y), 2 * (y * z + w * x), 1 - 2 * (x * x + y * y)));
	}

	inline float getX() const {
		return x;
	}

	inline float getY() const {
		return y;
	}

	inline float getZ() const {
		return z;
	}

	inline float getW() const {
		return w;
	}

	inline float getRadians() const {
		float angle;

		float dirW = w;

		if (w * w + y * y > 0.0f) {
			if (w > 0.f && y < 0.0f)
				dirW *= -1.0f;

			angle = 2.0f * Math::acos(dirW);
		} else {
			angle = 0.0f;
		}

		return angle;
	}

	inline Quaternion getConjugate() const {
		return Quaternion(w, -x, -y, -z);
	}

	inline float getSpecialDegrees() const { // returns 0-100 degrees
		return (getRadians() / 6.283f) * 100.f;
	}

	inline float getDegrees() const {
		return (getRadians() / 6.283f) * 360.f;
	}

	inline bool isIdentity() const {
		return (*this == IDENTITY);
	}

	inline void set(float fw, float fx, float fy, float fz) {
		w = fw;
		x = fx;
		y = fy;
		z = fz;
	}

	inline void setHeadingDirection(float radians) {
		float halfAngle = radians / 2;

		w = Math::cos(halfAngle);
		y = Math::sin(halfAngle);
	}
};

} // u3d
} // util
} // engine

using namespace engine::util::u3d;

#endif /* QUATERNION_H_ */
