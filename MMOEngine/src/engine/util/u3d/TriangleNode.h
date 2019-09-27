/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * TriangleNode.h
 *
 *  Created on: 11/03/2011
 *      Author: victor
 */

#ifndef TRIANGLENODE_H_
#define TRIANGLENODE_H_

#include "Triangle.h"
#include "system/util/Vector.h"

namespace engine {
 namespace util {
  namespace u3d {

  	  class TriangleNode : public Triangle {
  	  public:
		  virtual ~TriangleNode() {

		  }

  		  virtual uint32 getID() const = 0;
  		  virtual const Vector<TriangleNode*>* getNeighbors() const = 0;
  	  };

  }
 }
}

using namespace engine::util::u3d;


#endif /* TRIANGLENODE_H_ */
