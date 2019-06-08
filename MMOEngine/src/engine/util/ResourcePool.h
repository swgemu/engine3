/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef RESOURCEPOOL_H_
#define RESOURCEPOOL_H_

#include "system/lang.h"

namespace engine {
	namespace util {

	template<class R> class ResourcePool {
		Vector<R> resources;

	public:
		ResourcePool() { 
		}

		void initialize(const R& min, const R& max);

		// resource allocation methods
		R allocate();

		void free(R& res);

		// resource management methods
		/*void addResource(R& res);

		bool removeResource(R& res);*/

	};

	template<class R> void ResourcePool<R>::initialize(const R& min, const R& max) {
		for (R res = min; res <= max; ++res) {
			resources.add(res);
		}
	}

	template<class R> R ResourcePool<R>::allocate() {
		if (resources.size() == 0)
			throw ArrayIndexOutOfBoundsException(0);

		return resources.remove(0);
	}

	template<class R> void ResourcePool<R>::free(R& res) {
		resources.add(res);
	}

	} // namespace util
} // namespace engine

using namespace engine::util;

#endif /*RESOURCEPOOL_H_*/
