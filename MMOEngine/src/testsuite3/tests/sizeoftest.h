/*
 * sizeoftest.h
 *
 *  Created on: 11/09/2012
 *      Author: victor
 */

#ifndef SIZEOFTEST_H_
#define SIZEOFTEST_H_

#include "engine/util/Facade.h"

#define PRINT_OBJECT_SIZE(OBJ) \
			printf("size of " #OBJ " is:%ld\n", sizeof(OBJ));

#define SIZE_ASSERT(OBJ, MAX_SIZE) \
			assert(sizeof(OBJ) < MAX_SIZE);

void testObjectSizes() {
	//core
	PRINT_OBJECT_SIZE(ManagedObject);
	PRINT_OBJECT_SIZE(ManagedObjectImplementation);
	PRINT_OBJECT_SIZE(DistributedObject);
	PRINT_OBJECT_SIZE(DistributedObjectStub);
	PRINT_OBJECT_SIZE(Task);

	//util
	PRINT_OBJECT_SIZE(Observer);
	PRINT_OBJECT_SIZE(ObserverImplementation);
	PRINT_OBJECT_SIZE(Facade);
	PRINT_OBJECT_SIZE(FacadeImplementation);

	//u3d
	PRINT_OBJECT_SIZE(Coordinate);
	PRINT_OBJECT_SIZE(Quaternion);

	//thread
	PRINT_OBJECT_SIZE(Lockable);
	PRINT_OBJECT_SIZE(ReadWriteLock);
	PRINT_OBJECT_SIZE(Mutex);
	PRINT_OBJECT_SIZE(pthread_rwlock_t);

	//base
	PRINT_OBJECT_SIZE(Object);
	PRINT_OBJECT_SIZE(WeakReference<Object*>);
	PRINT_OBJECT_SIZE(Reference<Object*>);

	//util
	PRINT_OBJECT_SIZE(Vector<Object*>);
	PRINT_OBJECT_SIZE(ArrayList<Object*>);
	/*PRINT_OBJECT_SIZE(VectorMap);
	PRINT_OBJECT_SIZE(HashTable);*/
	PRINT_OBJECT_SIZE(String);

}

#endif /* SIZEOFTEST_H_ */
