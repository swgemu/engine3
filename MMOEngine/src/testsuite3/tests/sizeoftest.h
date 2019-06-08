/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * sizeoftest.h
 *
 *  Created on: 11/09/2012
 *      Author: victor
 */

#ifndef SIZEOFTEST_H_
#define SIZEOFTEST_H_

#include "engine/util/Facade.h"
#include "engine/util/u3d/Sphere.h"
#include "engine/util/u3d/Segment.h"
#include "engine/util/Observer.h"
#include "engine/util/u3d/Coordinate.h"

#define PRINT_OBJECT_SIZE(OBJ) \
			printf("size of " #OBJ " is: %ld bytes\n", sizeof(OBJ));

#define SIZE_ASSERT(OBJ, MAX_SIZE) \
			assert(sizeof(OBJ) < MAX_SIZE);

void testObjectSizes() {
	//basic
	PRINT_OBJECT_SIZE(char);
	PRINT_OBJECT_SIZE(bool);
	PRINT_OBJECT_SIZE(short);
	PRINT_OBJECT_SIZE(int);
	PRINT_OBJECT_SIZE(float);
	PRINT_OBJECT_SIZE(double);
	PRINT_OBJECT_SIZE(long);
	PRINT_OBJECT_SIZE(long long);

	//lang
	PRINT_OBJECT_SIZE(Variable);
	PRINT_OBJECT_SIZE(Object);
	PRINT_OBJECT_SIZE(WeakReference<Object*>);
	PRINT_OBJECT_SIZE(Reference<Object*>);
	PRINT_OBJECT_SIZE(String);
	PRINT_OBJECT_SIZE(StringBuffer);
	PRINT_OBJECT_SIZE(UnicodeString);
	PRINT_OBJECT_SIZE(Time);
	PRINT_OBJECT_SIZE(Byte);
	PRINT_OBJECT_SIZE(Integer);
	PRINT_OBJECT_SIZE(Bool);
	PRINT_OBJECT_SIZE(Character);
	PRINT_OBJECT_SIZE(Float);
	PRINT_OBJECT_SIZE(Long);
	PRINT_OBJECT_SIZE(Short);

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
	//PRINT_OBJECT_SIZE(QuadTree);
	//PRINT_OBJECT_SIZE(QuadTreeEntry);
	//PRINT_OBJECT_SIZE(QuadTreeEntryImplementation);
	PRINT_OBJECT_SIZE(Matrix3);
	PRINT_OBJECT_SIZE(Matrix4);
	PRINT_OBJECT_SIZE(Vector3);
	PRINT_OBJECT_SIZE(Vector4);
	PRINT_OBJECT_SIZE(Ray);
	PRINT_OBJECT_SIZE(Segment);
	PRINT_OBJECT_SIZE(Sphere);
	PRINT_OBJECT_SIZE(Triangle);
	PRINT_OBJECT_SIZE(TriangleNode);
	PRINT_OBJECT_SIZE(AABB);
	PRINT_OBJECT_SIZE(AABBNode);
	PRINT_OBJECT_SIZE(IntersectionResult);

	//thread
	PRINT_OBJECT_SIZE(Lockable);
	PRINT_OBJECT_SIZE(ReadWriteLock);
	PRINT_OBJECT_SIZE(Mutex);
	PRINT_OBJECT_SIZE(pthread_rwlock_t);
	PRINT_OBJECT_SIZE(pthread_mutex_t);
	PRINT_OBJECT_SIZE(pthread_mutexattr_t);
	PRINT_OBJECT_SIZE(Condition);
	PRINT_OBJECT_SIZE(Thread);
	PRINT_OBJECT_SIZE(Locker);
	PRINT_OBJECT_SIZE(ReadLocker);

	//atomic
	PRINT_OBJECT_SIZE(AtomicBoolean);
	PRINT_OBJECT_SIZE(AtomicInteger);
	PRINT_OBJECT_SIZE(AtomicLong);
	PRINT_OBJECT_SIZE(AtomicReference<Object*>);

	//util
	PRINT_OBJECT_SIZE(Vector<Object*>);
	PRINT_OBJECT_SIZE(ArrayList<Object*>);

	//preprocessor doesnt like commas in passing arguments
	typedef VectorMap<int, Object*> vectorMap;
	PRINT_OBJECT_SIZE(vectorMap);

	typedef HashTable<int, Object*> hashTable;
	PRINT_OBJECT_SIZE(hashTable);

	//io
	PRINT_OBJECT_SIZE(Serializable);
	PRINT_OBJECT_SIZE(File);
	PRINT_OBJECT_SIZE(Stream);
	PRINT_OBJECT_SIZE(ObjectInputStream);
	PRINT_OBJECT_SIZE(ObjectOutputStream);
	PRINT_OBJECT_SIZE(StringTokenizer);
	PRINT_OBJECT_SIZE(UnicodeTokenizer);

	//net
	PRINT_OBJECT_SIZE(Packet);
	PRINT_OBJECT_SIZE(Socket);
	PRINT_OBJECT_SIZE(SocketAddress);
	PRINT_OBJECT_SIZE(UDPSocket);
	PRINT_OBJECT_SIZE(TCPSocket);



}

#endif /* SIZEOFTEST_H_ */
