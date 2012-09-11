/*
 * sizeoftest.h
 *
 *  Created on: 11/09/2012
 *      Author: victor
 */

#ifndef SIZEOFTEST_H_
#define SIZEOFTEST_H_

#define PRINT_OBJECT_SIZE(OBJ) \
			printf("size of " #OBJ " is:%ld\n", sizeof(OBJ));

#define SIZE_ASSERT(OBJ, MAX_SIZE) \
			assert(sizeof(OBJ) < MAX_SIZE);

void testObjectSizes() {
	PRINT_OBJECT_SIZE(ManagedObject);
	PRINT_OBJECT_SIZE(ManagedObjectImplementation);
	PRINT_OBJECT_SIZE(Lockable);
	PRINT_OBJECT_SIZE(ReadWriteLock);
	PRINT_OBJECT_SIZE(pthread_rwlock_t);
	PRINT_OBJECT_SIZE(Object);
	PRINT_OBJECT_SIZE(WeakReference<Object*>);
	PRINT_OBJECT_SIZE(Mutex);
}

#endif /* SIZEOFTEST_H_ */
