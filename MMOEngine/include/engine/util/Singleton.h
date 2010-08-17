/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef SINGLETON_H_
#define SINGLETON_H_

#include "../../system/thread/ReadWriteLock.h"

namespace engine {
	namespace util {

	template<class O> class SingletonWrapper {
		O* inst;
		ReadWriteLock rwlock;
		bool finalized ;

	public:
		SingletonWrapper() {
			inst = NULL;
			finalized = false;
		}

		~SingletonWrapper() {
			finalize();
		}

		O* instance() {
			if (inst == NULL && !finalized) {
				rwlock.wlock();

				if (inst == NULL && !finalized)
					inst = new O();

				rwlock.unlock();
			}

			return inst;
		}

		void finalize() {
			rwlock.wlock();

			if (inst != NULL)
				delete inst;

			finalized = true;

			rwlock.unlock();

			inst = NULL;
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

		static void finalize() {
			getWrapper()->finalize();
		}

	};

	} // namespace util
} // namespace engine

using namespace engine::util;

#endif /*SINGLETON_H_*/
