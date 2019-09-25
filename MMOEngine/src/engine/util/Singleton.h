/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef SINGLETON_H_
#define SINGLETON_H_

#include "system/thread/ReadWriteLock.h"
#include "system/lang/ref/Reference.h"

namespace engine {
	namespace util {

	template<class O> class SingletonWrapper {
		Reference<O*> inst;
		bool finalized;

	public:
		SingletonWrapper() {
			inst = nullptr;
			finalized = false;
		}

		~SingletonWrapper() {
			finalize();
		}

		O* instance() {
			if (inst == nullptr && !finalized) {
				O* in = new O();

				if (!inst.compareAndSet(nullptr, in))
					delete in;
			}

			return inst;
		}

		void setInstance(O* instance) {
			inst = instance;

			finalized = false;
		}

		void finalize() {
			finalized = true;

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
