/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ALLOCATIONTRACKER_H_
#define ALLOCATIONTRACKER_H_

#include "system/lang/Time.h"

#include "system/thread/Mutex.h"
#include "system/thread/Locker.h"

#include "Allocator.h"
#include "AllocationHook.h"

#define MM_ALLOCATOR_LIST_SIZE 4096

namespace sys {
  namespace mm {

	class AllocationTracker : public AllocationHook {
		//Allocator* allocator;

		pthread_mutex_t mutex;

		struct MM_Allocator
		{
			unsigned long caller;
			long          total;
		};

		unsigned int numOfAllocatorsM ;

		MM_Allocator allocatorsM[MM_ALLOCATOR_LIST_SIZE];

	public:
		AllocationTracker(Allocator* alloc) {
			pthread_mutex_init(&mutex, NULL);

			//allocator = alloc;

			numOfAllocatorsM = 0;

			memset(allocatorsM, 0, sizeof(allocatorsM));

		}

		~AllocationTracker() {
			pthread_mutex_destroy(&mutex);
		}

		void* onAllocate(size_t size, const void* allocator) {
			pthread_mutex_lock(&mutex);

			size += 16;

			unsigned long* ptr = (unsigned long*) unHookedAllocate(size);

			/*eventCounter.counterEventOccured(0);
			eventCounter.mapEventOccured((unsigned long) allocator, size);*/

			updateAllocatorList((unsigned long) allocator, size);

		    *(ptr++) = (unsigned long) allocator;
		    *(ptr++) = (unsigned long) size;

			pthread_mutex_unlock(&mutex);

		    return ptr;
		}

		void onFree(void* ptr, const void*) {
			if (ptr == NULL)
				return;

			pthread_mutex_lock(&mutex);

			unsigned long* lptr = (unsigned long*) ptr;

		    size_t size =*(--lptr);
		    void* allocator = (void*) *(--lptr);

		    /*eventCounter.counterEventOccured(1);
		    eventCounter.mapEventOccured((unsigned long)allocator, -size);*/

		    updateAllocatorList((unsigned long) allocator, -size);

		   	unHookedFree((void*) lptr);

			pthread_mutex_unlock(&mutex);
		}

		void* onReallocate(void* ptr, size_t size, const void* alloc) {
			if (ptr == NULL)
				return onAllocate(size, alloc);

			pthread_mutex_lock(&mutex);

			unsigned long* lptr = (unsigned long*) ptr;

			size += 16;

			size_t old_chunk_size = *(--lptr);
			void* allocator = (void*) *(--lptr);

			void* ptr2 = unHookedReallocate((void*)lptr, size);

			unsigned long* nlptr = (unsigned long*) ptr2;
			*(nlptr++) = (unsigned long) allocator;
			*(nlptr++) = size;

			/*eventCounter.counterEventOccured(2);
			eventCounter.mapEventOccured((unsigned long)allocator, -(old_chunk_size - size));*/

			updateAllocatorList((unsigned long)allocator, -(old_chunk_size - size));

			pthread_mutex_unlock(&mutex);

			return nlptr;
		}

		Time nextAllocatorPrint;

		void updateAllocatorList(unsigned long allocator, long size) {
			for(unsigned int i = 0; i < numOfAllocatorsM; i++)
			{
				if(allocatorsM[i].caller == allocator)
				{
					 allocatorsM[i].total += size;
					 return;
				}
			}
			if(numOfAllocatorsM < MM_ALLOCATOR_LIST_SIZE)
			{
				allocatorsM[numOfAllocatorsM].caller = allocator;
				allocatorsM[numOfAllocatorsM].total  = size;
				numOfAllocatorsM++;
			}
			else
			{
				printf("Allocator debug, too many allocators!!!!\n");
			}

			if (nextAllocatorPrint.isPast()) {
				print();
				nextAllocatorPrint.addMiliTime(10000);
			}
		}

		void print() {
			printf("--- ALLOCATORS ---\n");
			unsigned long grand_total = 0;
			for(unsigned int i = 0; i < numOfAllocatorsM; i++)
			{
				if(allocatorsM[i].total > 0)
				{
					printf("#%4u 0x%08lx %ld\n", i, allocatorsM[i].caller, allocatorsM[i].total);
				}
				else if(allocatorsM[i].total < 0)
				{
					printf("#%4u 0x%08lx %ld ???\n", i, allocatorsM[i].caller, allocatorsM[i].total);
				}
				grand_total += allocatorsM[i].total;
			}
			printf("Total: %lu (%u)\n", grand_total, numOfAllocatorsM);
			printf("--- ALLOCATORS ---\n");
			fflush(stdout);
		}
	};

  } // namespace mm
} // namespace sys

using namespace sys::mm;

#endif /* ALLOCATIONTRACKER_H_ */
