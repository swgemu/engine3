/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
//
// Created by theanswer on 1/2/18.
//

#ifndef ENGINE3_MPSCNODE_H
#define ENGINE3_MPSCNODE_H

#include <atomic>

namespace sys {
	namespace util {
			template<typename Data>
			class MPSCNode {
				public:
					MPSCNode(const Data& a) {
						data = a;
					}

					MPSCNode()  = default;
					MPSCNode(MPSCNode<Data>&&)  = default;

					Data data;

					std::atomic<MPSCNode<Data>*> next;
			};
	}
}

#endif //ENGINE3_MPSCNODE_H
