/*
 * ObserverImplementation.cpp
 *
 *  Created on: 20/06/2010
 *      Author: victor
 */

#include "Observer.h"

uint64 ObserverImplementation::getObjectID() const {
	return _this.get()->_getObjectID();
}
