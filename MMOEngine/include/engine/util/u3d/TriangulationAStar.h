/*
 * TriangulationAStar.h
 *
 *  Created on: 06/03/2011
 *      Author: victor
 */

#ifndef TRIANGULATIONASTAR_H_
#define TRIANGULATIONASTAR_H_

#include "AStarAlgorithm.h"
#include "Triangle.h"
#include "Vector3.h"

namespace engine {
 namespace util {
  namespace u3d {

  class TriangleChannel {
  	int numEdges;

  	Vector<TriangleNode*> triangles;

  public:
  	TriangleChannel() {
  		numEdges = 0;
  	}

  	inline void addTriangle(TriangleNode* node) {
  		triangles.add(node);

  		if (node->hasEdges())
  			numEdges++;
  	}
  };

  template<class Graph, class Node>
  class TriangulationAStar {

  public:

  	//static Vector<Vector3>* funnel(Vector<TriangleNode*> channel, const Vector3& startPoint, const Vector3& goalPoint);

  };




  }
 }
}

using namespace engine::util::u3d;

#endif /* TRIANGULATIONASTAR_H_ */
