/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef MULTIMAPMEMORYMANAGER_H_
#define MULTIMAPMEMORYMANAGER_H_

#include "system/platform.h"

#include "system/lang/Object.h"

#include "system/thread/atomic/AtomicReference.h"
#include "system/thread/atomic/AtomicInteger.h"

namespace sys {
  namespace mm {

	class MultimapMemoryManager : public Object {
		static AtomicReference<MultimapMemoryManager*> inst;

		int deviceFD;

		static AtomicInteger heapCount;

	public:
		~MultimapMemoryManager();

		static MultimapMemoryManager* getInstance() {
			if (inst == nullptr) {
				setInstance(new MultimapMemoryManager());

				inst->initialize();
			}

			return inst;
		}

		static void setInstance(MultimapMemoryManager* manager) {
			inst.set(manager);
		}

		void* createHeapBase(size_t size);

		void* getKernelHeapBase();

		void setKernelMemoryProtection(unsigned long long heapBase, volatile unsigned& protectionLevel, unsigned desiredKEL);

		int createHeap();

		void selectHeap(int heapID);

		void destroyHeap(int heapID);

		int getDeviceFD() {
			return deviceFD;
		}

	private:
		void initialize();
		void initializeKernel();

		void openDevice(unsigned deviceNumber);
	};

  } // namespace mm
} // namespace sys

using namespace sys::mm;

#endif /*MULTIMAPMEMORYMANAGER_H_*/
