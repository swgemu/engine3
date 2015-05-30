/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef COORDINATE_H_
#define COORDINATE_H_

#include "Vector3.h"

namespace engine {
  namespace util {
  	namespace u3d {

	class Coordinate : public virtual Object {
	protected:
		float positionX, positionZ, positionY;

		float previousPositionX, previousPositionZ, previousPositionY;

	public:
		Coordinate() : positionX(0), positionZ(0), positionY(0),
					previousPositionX(0), previousPositionZ(0), previousPositionY(0) {

		}

		Coordinate(const Coordinate& ct) : Object() {
			positionX = ct.positionX;
			positionY = ct.positionY;
			positionZ = ct.positionZ;

			previousPositionX = ct.previousPositionX;
			previousPositionY = ct.previousPositionY;
			previousPositionZ = ct.previousPositionZ;
		}

		Coordinate(float x, float z, float y) {
			positionX = x;
			positionZ = z;
			positionY = y;

			updatePreviousPosition();
		}

		Coordinate& operator=(const Coordinate& ct) {
			if (this == &ct)
				return *this;

			positionX = ct.positionX;
			positionY = ct.positionY;
			positionZ = ct.positionZ;

			previousPositionX = ct.previousPositionX;
			previousPositionY = ct.previousPositionY;
			previousPositionZ = ct.previousPositionZ;

			return *this;
		}

		virtual ~Coordinate() {
		}

		inline void initializePosition(float x, float z, float y) {
			positionX = x;
			positionZ = z;
			positionY = y;

			updatePreviousPosition();
		}

		inline void initializePosition(const Vector3& pos) {
			positionX = pos.getX();
			positionZ = pos.getZ();
			positionY = pos.getY();

			updatePreviousPosition();
		}

		bool toBinaryStream(ObjectOutputStream* stream) {
			stream->writeFloat(positionX);
			stream->writeFloat(positionZ);
			stream->writeFloat(positionY);

			stream->writeFloat(previousPositionX);
			stream->writeFloat(previousPositionZ);
			stream->writeFloat(previousPositionY);

			return true;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			positionX = stream->readFloat();
			positionZ = stream->readFloat();
			positionY = stream->readFloat();

			previousPositionX = stream->readFloat();
			previousPositionZ = stream->readFloat();
			previousPositionY = stream->readFloat();

			return true;
		}

		void randomizePosition(float radius) {
			updatePreviousPosition();

			float angle = (45 + System::random(200)) / 3.14;
			float distance = radius + System::random((int) radius);

			positionX += cos(angle) * distance;
			positionY += sin(angle) * distance;
		}

		inline void setPosition(float x, float z, float y) {
			updatePreviousPosition();

			positionX = x;
			positionZ = z;
			positionY = y;
		}

		inline void setPosition(const Vector3& pos) {
			updatePreviousPosition();

			positionX = pos.getX();
			positionZ = pos.getZ();
			positionY = pos.getY();
		}

		inline void setPositionX(float x) {
			previousPositionX = positionX;

			positionX = x;
		}

		inline void setPositionZ(float z) {
			previousPositionZ = positionZ;

			positionZ = z;
		}

		inline void setPositionY(float y) {
			previousPositionY = positionY;

			positionY = y;
		}

		inline void updatePreviousPosition() {
			previousPositionX = positionX;
			previousPositionZ = positionZ;
			previousPositionY = positionY;
		}

		// getters
		inline float getPositionX() const {
			return positionX;
		}

		inline float getPositionZ() const {
			return positionZ;
		}

		inline float getPositionY() const {
			return positionY;
		}

		inline float getPreviousPositionX() const {
			return previousPositionX;
		}

		inline float getPreviousPositionZ() const {
			return previousPositionZ;
		}

		inline float getPreviousPositionY() const {
			return previousPositionY;
		}

		inline Vector3 getPosition() const {
			return Vector3(positionX, positionY, positionZ);
		}

	};

  	} // u3d
  } // namespace util
} // namespace engine

using namespace engine::util::u3d;

#endif /*COORDINATE_H_*/
