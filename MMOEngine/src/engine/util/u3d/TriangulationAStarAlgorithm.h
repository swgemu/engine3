/*
 * TriangulationAStarAlgorithm.h
 *
 *  Created on: 11/03/2011
 *      Author: victor
 */

#ifndef TRIANGULATIONASTARALGORITHM_H_
#define TRIANGULATIONASTARALGORITHM_H_

#include "AStarAlgorithm.h"
#include "TriangleNode.h"

namespace engine {
 namespace util {
  namespace u3d {

  class TriangulationAStarAlgorithm {
  public:
	  static Vector<Triangle*>* search(const Vector3& startPoint, const Vector3& goalPoint, TriangleNode* source, TriangleNode* target);
  };



  }
 }
}

using namespace engine::util::u3d;


#endif /* TRIANGULATIONASTARALGORITHM_H_ */
