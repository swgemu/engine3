/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef COORDINATE_H_
#define COORDINATE_H_

#include "Vector3.h"

namespace engine {
  namespace util {
  	namespace u3d {

	class Coordinate {
	protected:
		Vector3 position;
		Vector3 previousPosition;

	public:
		Coordinate() {
		}

		Coordinate(const Coordinate& ct) {
			position = ct.position;

			previousPosition = ct.previousPosition;
		}

		Coordinate(float x, float z, float y) {
			position.set(x, z, y);

			updatePreviousPosition();
		}

		Coordinate& operator=(const Coordinate& ct) {
			if (this == &ct)
				return *this;

			position = ct.position;
			previousPosition = ct.previousPosition;

			return *this;
		}

		inline void initializePosition(float x, float z, float y) {
			position.set(x, z, y);

			updatePreviousPosition();
		}

		inline void initializePosition(const Vector3& pos) {
			position = pos;

			updatePreviousPosition();
		}

		bool toBinaryStream(ObjectOutputStream* stream) {
			position.toBinaryStream(stream);
			previousPosition.toBinaryStream(stream);

			return true;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			position.parseFromBinaryStream(stream);
			previousPosition.parseFromBinaryStream(stream);

			return true;
		}

		void randomizePosition(float radius) {
			updatePreviousPosition();

			float angle = (45 + System::random(200)) / 3.14;
			float distance = radius + System::random((int) radius);

			position.setX(position.getX() + cos(angle) * distance);
			position.setY(position.getY() + sin(angle) * distance);
		}

		inline void setPosition(float x, float z, float y) {
			updatePreviousPosition();

			position.set(x, z, y);
		}

		inline void setPosition(const Vector3& pos) {
			updatePreviousPosition();

			position = pos;
		}

		inline void setPositionX(float x) {
			previousPosition.setX(x);

			position.setX(x);
		}

		inline void setPositionZ(float z) {
			previousPosition.setZ(z);

			position.setZ(z);
		}

		inline void setPositionY(float y) {
			previousPosition.setY(y);

			position.setY(y);
		}

		inline void updatePreviousPosition() {
			previousPosition = position;
		}

		// getters
		inline float getPositionX() const {
			return position.getX();
		}

		inline float getPositionZ() const {
			return position.getZ();
		}

		inline float getPositionY() const {
			return position.getY();
		}

		inline float getPreviousPositionX() const {
			return previousPosition.getX();
		}

		inline float getPreviousPositionZ() const {
			return previousPosition.getZ();
		}

		inline float getPreviousPositionY() const {
			return previousPosition.getY();
		}

		inline const Vector3& getPosition() const {
			return position;
		}

		inline const Vector3& getPreviousPosition() const {
			return previousPosition;
		}

		inline Vector3& getPosition(){
			return position;
		}

		inline Vector3& getPreviousPosition() {
			return previousPosition;
		}

	};

  	} // u3d
  } // namespace util
} // namespace engine

using namespace engine::util::u3d;

#endif /*COORDINATE_H_*/
