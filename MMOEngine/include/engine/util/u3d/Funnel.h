/*
 * Funnel.h
 *
 *  Created on: 07/03/2011
 *      Author: victor
 */

#ifndef FUNNEL_H_
#define FUNNEL_H_

#include "Vector3.h"
#include "Triangle.h"

namespace engine {
 namespace util {
  namespace u3d {

  class Funnel {
  public:
  	static Vector<Vector3>* funnel(const Vector3& startPoint, const Vector3& goalPoint, Vector<Triangle*>* channel);
  };

  }
 }
}

using namespace engine::util::u3d;


#endif /* FUNNEL_H_ */
