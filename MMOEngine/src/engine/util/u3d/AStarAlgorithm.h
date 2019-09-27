/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * AStarAlgorithm.h
 *
 *  Created on: 06/02/2011
 *      Author: victor
 */

#ifndef ASTARALGORITHM_H_
#define ASTARALGORITHM_H_

#include "system/util/PriorityQueue.h"
#include "system/lang.h"
#include "system/platform.h"
#include "system/util/VectorMap.h"

namespace engine {
 namespace util {
  namespace u3d {

	template <class Node, class IDType>
	class AStarNode : public PriorityQueueEntry, public Object {
		float g, heuristic;

		const Node* node;

		const AStarNode<Node, IDType>* cameFrom;
	public:
		AStarNode(const Node* node, float g, float heuristic) {
			AStarNode::g = g;
			AStarNode::heuristic = heuristic;
			AStarNode::node = node;

			cameFrom = nullptr;
		}

		int compareTo(const PriorityQueueEntry* node) const {
			const AStarNode<Node, IDType>* cmp = static_cast<const AStarNode<Node, IDType>*>(node);

			if (getF() < cmp->getF())
				return 1;
			else if (getF() > cmp->getF())
				return -1;
			else
				return 0;
		}

		inline void setCameFrom(const AStarNode<Node, IDType>* star) {
			cameFrom = star;
		}

		inline void setG(float g) {
			this->g = g;
		}

		inline void setHeuristic(float h) {
			heuristic = h;
		}

		inline const Node* getNode() const {
			return node;
		}

		inline IDType getID() const {
			return node->getID();
		}

		inline const AStarNode<Node, IDType>* getCameFrom() const {
			return cameFrom;
		}

		inline float getF() const {
			return g + heuristic;
		}

		inline float getG() const {
			return g;
		}

		inline float getHeuristic() const {
			return heuristic;
		}


	};

	template<class Graph, class Node>
	class AStarAlgorithm {
	public:
		template <class IDType>
		static Vector<const Node*>* search(const Graph* graph, const Node* source, const Node* target) {
			VectorMap<IDType, Reference<AStarNode<Node, IDType>* > > openSet;
			openSet.setAllowDuplicateInsertPlan();
			openSet.setNullValue(nullptr);

			PriorityQueue priorityQueue;
			VectorMap<IDType, Reference<AStarNode<Node, IDType>* > > closeSet;
			closeSet.setAllowDuplicateInsertPlan();
			closeSet.setNullValue(nullptr);

			Reference<AStarNode<Node, IDType>* > start = new AStarNode<Node, IDType>(source, 0, graph->calculateManhattanDistance(source, target));
			openSet.put(source->getID(), start);
			priorityQueue.add(start);

			Reference<AStarNode<Node, IDType>* > goal = nullptr;

			while (openSet.size() > 0) {
				Reference<AStarNode<Node, IDType>* > x = static_cast<AStarNode<Node, IDType>*>(priorityQueue.poll());
				openSet.drop(x->getID());

				if (TypeInfo<IDType>::compare(x->getID(), target->getID()) == 0) {
				//if (x->getID() == target->getID()) {
					goal = x;

					break;
				} else {
					closeSet.put(x->getID(), x);

					const Vector<Node*>* neighbors = x->getNode()->getNeighbors();

					for (int i = 0; i < neighbors->size(); ++i) {
						const Node* neighbor = neighbors->getUnsafe(i);

						const Reference<AStarNode<Node, IDType>* >& visited = closeSet.get(neighbor->getID());

						if (visited == nullptr) {
							float g = x->getG() + graph->calculateManhattanDistance(x->getNode(), neighbor);

							AStarNode<Node, IDType>* n = openSet.get(neighbor->getID()).get();

							if (n == nullptr) {
								n = new AStarNode<Node, IDType>(neighbor, g, graph->calculateManhattanDistance(neighbor, target));
								n->setCameFrom(x);
								openSet.put(neighbor->getID(), n);

								priorityQueue.add(n);
							} else if (g < n->getG()) {
								n->setCameFrom(x);
								n->setG(g);
								n->setHeuristic(graph->calculateManhattanDistance(neighbor, target));
							}
						}
					}
				}
			}

			if (goal == nullptr)
				return nullptr;

			auto path = new Vector<const Node*>();
			path->add(goal->getNode());

			const AStarNode<Node, IDType>* parent = goal->getCameFrom();

			while (parent != nullptr) {
				path->insertElementAt(parent->getNode(), 0);
				parent = parent->getCameFrom();
			}

			return path;
		}
	};

  }
 }
}

using namespace engine::util::u3d;



#endif /* ASTARALGORITHM_H_ */
