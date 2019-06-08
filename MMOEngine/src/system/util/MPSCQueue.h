/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
//
// Created by theanswer on 1/2/18.
//

#ifndef ENGINE3_MPSCQUEUE_H
#define ENGINE3_MPSCQUEUE_H

#include "MPSCNode.h"


/**
 * Wait free Multiple Producers Single Consumer Queue
 * http://www.1024cores.net/home/lock-free-algorithms/queues/non-intrusive-mpsc-node-based-queue
 */
namespace sys {
	namespace util {
		template<class A>
		class MPSCQueue {
		private:
			std::atomic<MPSCNode<A>*> head;
			MPSCNode<A>* tail;

		public:
			MPSCQueue(int capacity = 0/*unused*/) {
				auto head = new MPSCNode<A>();

				this->head = head;
				head->next = nullptr;

				tail = head;
			}

			~MPSCQueue() {
				delete tail;
			}

			void push(const A& data) {
				push(new MPSCNode<A>(data));
			}

			void push(A&& data) {
				push(new MPSCNode<A>(std::move(data)));
			}

			void push(MPSCNode<A>* n) {
				n->next = nullptr;

				MPSCNode<A>* prev = std::atomic_exchange_explicit(&this->head, n, std::memory_order_acq_rel);
				prev->next.store(n, std::memory_order_release);
			}

			bool popValue(A& val) {
				auto node = popNode();

				if (node != nullptr) {
					val = node->data;

					delete node;

					return true;
				}

				return false;
			}

			MPSCNode<A>* popNode() {
				MPSCNode<A>* tail = this->tail;
				MPSCNode<A>* next = tail->next.load(std::memory_order_acquire);

				if (next) {
					this->tail = next;
					tail->data = std::move(next->data);
					return tail;
				}

				return nullptr;
			}
		};
	}
}

#endif //ENGINE3_MPSCQUEUE_H
