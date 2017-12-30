/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef SINGLETON_H_
#define SINGLETON_H_

#include "system/thread/ReadWriteLock.h"
#include "system/lang/ref/Reference.h"

namespace engine {
	namespace util {

	template<class O> class SingletonWrapper {
		Reference<O*> inst;
		//pthread_rwlock_t* rwlock;
		bool finalized;

	public:
		SingletonWrapper() {
			inst = nullptr;
			finalized = false;
			/*rwlock = (pthread_rwlock_t*) malloc(sizeof(pthread_rwlock_t));
			pthread_rwlock_init(rwlock, nullptr);*/
		}

		~SingletonWrapper() {
			finalize();

			//free(rwlock);
			//rwlock = nullptr;
		}

		O* instance() {
			if (inst == nullptr && !finalized) {
				/*int res = pthread_rwlock_wrlock(rwlock);//;rwlock.wlock();

				if (res != 0)
					System::out << "unlock failed on RWLock Singleton (" << res << ")\n";*/

				//if (inst == nullptr && !finalized)
					O* in = new O();

					if (!inst.compareAndSet(nullptr, in))
						delete in;

				//pthread_rwlock_unlock(rwlock);
			}

			return inst;
		}

		void setInstance(O* instance) {
			inst = instance;

			finalized = false;
		}

		void finalize() {
			//pthread_rwlock_wrlock(rwlock);//;rwlock.wlock();

			/*if (inst != nullptr)
				delete inst;*/

			finalized = true;

			//pthread_rwlock_unlock(rwlock);

			inst = nullptr;
		}

	};

	template<class O> class Singleton {
		static SingletonWrapper<O>* getWrapper() {
			static SingletonWrapper<O> wrapper;
			return &wrapper;
		}

	public:
		Singleton() {
		}

		static O* instance() {
			return getWrapper()->instance();
		}

		static void finalizeInstance() {
			getWrapper()->finalize();
		}

		static void setSingletonInstance(O* inst) {
			getWrapper()->setInstance(inst);
		}

	};

	} // namespace util
} // namespace engine

using namespace engine::util;

#endif /*SINGLETON_H_*/
