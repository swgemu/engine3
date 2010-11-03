/*
 * DistributedObjectMap.h
 *
 *  Created on: Oct 14, 2010
 *      Author: oru
 */

#ifndef DISTRIBUTEDHELPEROBJECTMAP_H_
#define DISTRIBUTEDHELPEROBJECTMAP_H_

#include "object/DistributedObject.h"
#include "system/lang.h"

namespace engine {
  namespace ORB {

	class DistributedHelperObjectMap : public HashTable<uint64, Reference<DistributedObject*> > {
		int hash(const uint64& key) {
			return Long::hashCode(key);
		}

	public:
		DistributedHelperObjectMap() : HashTable<uint64, Reference<DistributedObject*> >(30000) {
			setNullValue(NULL);
		};
	};

  }
}

using namespace engine::ORB;

#endif /* DISTRIBUTEDHELPEROBJECTMAP_H_ */
