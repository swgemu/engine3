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

		Node* node;

		Reference<AStarNode<Node, IDType>* > cameFrom;
	public:
		AStarNode(Node* node, float g, float heuristic) {
			AStarNode::g = g;
			AStarNode::heuristic = heuristic;
			AStarNode::node = node;

			cameFrom = NULL;
		}

		int compareTo(PriorityQueueEntry* node) {
			AStarNode<Node, IDType>* cmp = (AStarNode<Node, IDType>*)node;

			if (getF() < cmp->getF())
				return 1;
			else if (getF() > cmp->getF())
				return -1;
			else
				return 0;
		}

		inline void setCameFrom(AStarNode<Node, IDType>* star) {
			cameFrom = star;
		}

		inline void setG(float g) {
			this->g = g;
		}

		inline void setHeuristic(float h) {
			heuristic = h;
		}

		inline Node* getNode() {
			return node;
		}

		inline IDType getID() {
			return node->getID();
		}

		inline AStarNode<Node, IDType>* getCameFrom() {
			return cameFrom;
		}

		inline float getF() {
			return g + heuristic;
		}

		inline float getG() {
			return g;
		}

		inline float getHeuristic() {
			return heuristic;
		}


	};

	template<class Graph, class Node>
	class AStarAlgorithm {
	public:
		template <class IDType>
		static Vector<Node*>* search(Graph* graph, Node* source, Node* target) {
			VectorMap<IDType, Reference<AStarNode<Node, IDType>* > > openSet;
			openSet.setAllowDuplicateInsertPlan();
			openSet.setNullValue(NULL);

			PriorityQueue priorityQueue;
			VectorMap<IDType, Reference<AStarNode<Node, IDType>* > > closeSet;
			closeSet.setAllowDuplicateInsertPlan();
			closeSet.setNullValue(NULL);

			Reference<AStarNode<Node, IDType>* > start = new AStarNode<Node, IDType>(source, 0, graph->calculateManhattanDistance(source, target));
			openSet.put(source->getID(), start);
			priorityQueue.add(start);

			Reference<AStarNode<Node, IDType>* > goal = NULL;

			while (openSet.size() > 0) {
				Reference<AStarNode<Node, IDType>* > x = (AStarNode<Node, IDType>*)const_cast<PriorityQueueEntry*>(priorityQueue.poll());
				openSet.drop(x->getID());

				if (TypeInfo<IDType>::compare(x->getID(), target->getID()) == 0) {
				//if (x->getID() == target->getID()) {
					goal = x;

					break;
				} else {
					closeSet.put(x->getID(), x);

					Vector<Node*>* neighbors = x->getNode()->getNeighbors();

					for (int i = 0; i < neighbors->size(); ++i) {
						Node* neighbor = neighbors->get(i);

						Reference<AStarNode<Node, IDType>* > visited = closeSet.get(neighbor->getID());

						if (visited == NULL) {
							float g = x->getG() + graph->calculateManhattanDistance(x->getNode(), neighbor);

							Reference<AStarNode<Node, IDType>* > n = openSet.get(neighbor->getID());

							if (n == NULL) {
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

			if (goal == NULL)
				return NULL;

			Vector<Node*>* path = new Vector<Node*>();
			path->add(goal->getNode());

			AStarNode<Node, IDType>* parent = goal->getCameFrom();

			while (parent != NULL) {
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
