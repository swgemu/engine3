/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef COORDINATE_H_
#define COORDINATE_H_

namespace engine {
  namespace util {

	class Coordinate {
	protected:
		float positionX, positionZ, positionY;

		float previousPositionX, previousPositionZ, previousPositionY;

	public:
		Coordinate() {

		}

		Coordinate(float x, float z, float y) {
			positionX = x;
			positionZ = z;
			positionY = y;

			updatePreviousPosition();
		}

		virtual ~Coordinate() {
		}

		inline void initializePosition(float x, float z, float y) {
			positionX = x;
			positionZ = z;
			positionY = y;

			updatePreviousPosition();
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
		inline float getPositionX() {
			return positionX;
		}

		inline float getPositionZ() {
			return positionZ;
		}

		inline float getPositionY() {
			return positionY;
		}

		inline float getPreviousPositionX() {
			return previousPositionX;
		}

		inline float getPreviousPositionZ() {
			return previousPositionZ;
		}

		inline float getPreviousPositionY() {
			return previousPositionY;
		}

	};

  } // namespace util
} // namespace engine

using namespace engine::util;

#endif /*COORDINATE_H_*/
