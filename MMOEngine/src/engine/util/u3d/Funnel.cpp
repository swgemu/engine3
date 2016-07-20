/*
 * Funnel.cpp
 *
 *  Created on: 07/03/2011
 *      Author: victor
 */


#include "system/platform.h"

#include "Funnel.h"

Vector<Vector3>* Funnel::funnel(const Vector3& startPoint, const Vector3& goalPoint, Vector<Triangle*>* triangleChannel) {
	Vector<Vector3>* path = new Vector<Vector3>();

	path->add(startPoint);

	int apexIndex = 0, leftIndex = 0, rightIndex = 0;

	Vector3 portalApex = startPoint;
	Vector3 portalLeft = startPoint;
	Vector3 portalRight = startPoint;

	for (int i = 0; i < triangleChannel->size(); ++i) {
		Vector3 left;
		Vector3 right;

		if (i == triangleChannel->size() - 1) {
			 left = goalPoint;
			 right = goalPoint;
		} else {
			left = triangleChannel->get(i)->getLeftSharedVertex(triangleChannel->get(i + 1));
			right = triangleChannel->get(i)->getRightSharedVertex(triangleChannel->get(i + 1));
		}

		if (Triangle::area2D(portalApex, portalRight, right) <= 0.0f) {
			if (portalApex == portalRight || Triangle::area2D(portalApex, portalLeft, right) > 0.0f) {
				// Tighten the funnel.
				portalRight = right;

				rightIndex = i;
			} else {
				// Right over left, insert left to path and restart scan from portal left point.

				path->add(portalLeft);
				// Make current left the new apex.
				portalApex = portalLeft;

				apexIndex = leftIndex;
				// Reset portal
				portalLeft = portalApex;
				portalRight = portalApex;

				leftIndex = apexIndex;
				rightIndex = apexIndex;
				// Restart scan
				i = apexIndex;
				continue;
			}
		}

		// Update left vertex.
		if (Triangle::area2D(portalApex, portalLeft, left) >= 0.0f) {
			if (portalApex == portalLeft || Triangle::area2D(portalApex, portalRight, left) < 0.0f) {
				// Tighten the funnel.

				portalLeft = left;
				leftIndex = i;
			} else {
				// Left over right, insert right to path and restart scan from portal right point.

				path->add(portalRight);
				// Make current right the new apex.

				portalApex = portalRight;

				apexIndex = rightIndex;
				// Reset portal

				portalLeft = portalApex;

				portalRight = portalApex;
				leftIndex = apexIndex;
				rightIndex = apexIndex;
				// Restart scan
				i = apexIndex;
				continue;
			}
		}
	}
	
	path->add(goalPoint);

	return path;
}
