/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef SINGLETON_H_
#define SINGLETON_H_

#include "system/lang.h"

namespace engine {
	namespace util {

	template<class O> class SingletonWrapper {
		O* inst;

	public:
		SingletonWrapper() {
			inst = NULL;
		}

		~SingletonWrapper() {
			finalize();
		}

		O* instance() {
			if (inst == NULL)
				inst = new O();

			return inst;
		}

		void finalize() {
			if (inst != NULL)
				delete inst;

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
