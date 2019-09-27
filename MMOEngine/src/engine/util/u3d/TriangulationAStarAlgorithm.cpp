/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * TriangulationAStarAlgorithm.cpp
 *
 *  Created on: 11/03/2011
 *      Author: victor
 */

//based on http://skatgame.net/mburo/ps/thesis_demyen_2006.pdf

#include "TriangulationAStarAlgorithm.h"
#include "Segment.h"

Vector<const Triangle*>* TriangulationAStarAlgorithm::search(const Vector3& startPoint, const Vector3& goalPoint, const TriangleNode* source, const TriangleNode* target) {
	VectorMap<uint32, AStarNode<TriangleNode, uint32>* > visited(100, 10);
	visited.setNullValue(nullptr);
	visited.setNoDuplicateInsertPlan();

	PriorityQueue priorityQueue;

	AStarNode<TriangleNode, uint32>* start = new AStarNode<TriangleNode, uint32>(source, 0, startPoint.distanceTo(goalPoint));
	priorityQueue.add(start);

	AStarNode<TriangleNode, uint32>* goal = nullptr;

	while (!priorityQueue.isEmpty() && goal == nullptr) {
		AStarNode<TriangleNode, uint32>* x = static_cast<AStarNode<TriangleNode, uint32>*>(const_cast<PriorityQueueEntry*>(priorityQueue.poll()));

		if (target == x->getNode()) {
			goal = x;
		} else {
			auto neighbors = x->getNode()->getNeighbors();

			for (int i = 0; i < neighbors->size(); ++i) {
				const TriangleNode* neighbor = neighbors->getUnsafe(i);

				if (visited.contains(neighbor->getID()))
					continue;

				const Triangle* triangleA = x->getNode();
				const Triangle* triangleB = neighbor;

				const Vector3* pointA = &Vector3::ZERO;
				const Vector3* pointB = &Vector3::ZERO;

				triangleA->getSharedVertices(*triangleB, pointA, pointB);

				Vector3 closestPoint = Segment::getClosestPoint(*pointA, *pointB, goalPoint);

				float h = closestPoint.distanceTo(goalPoint);

				closestPoint = Segment::getClosestPoint(*pointA, *pointB, startPoint);

				float distEA = closestPoint.distanceTo(startPoint);

				//funnel distance
				//replacing with straight line for performance

				//funnel

/*
				Vector<Triangle*> currentPath;
				currentPath.add(neighbor);


				AStarNode<TriangleNode, uint32>* parent = x;

				while (parent != nullptr) {
					currentPath.insertElementAt(parent->getNode(), 0);

					if (parent != parent->getCameFrom())
						parent = parent->getCameFrom();
				}

				Vector<Vector3>* funnelPath = Funnel::funnel(startPoint, closestPoint, &currentPath);

				float funnelPathDistance = 0;

				if (funnelPath->size() > 1) {
					for (int i = 1; i < funnelPath->size(); ++i) {
						Vector3* firstPoint = &funnelPath->get(i - 1);
						Vector3* secondPoint = &funnelPath->get(i);

						funnelPathDistance += secondPoint->distanceTo(*firstPoint);
					}
				}

				delete funnelPath;

*/
				//

				//printf("x->getG():%f distEA:%f funnelPathDistance:%f heuristic:%f\n", x->getG(), distEA, funnelPathDistance, h);
	//			float g = MAX(MAX(x->getG(), distEA), funnelPathDistance);

				float g = Math::max(Math::max(x->getG(), distEA), x->getG() + (x->getHeuristic() - h));

				AStarNode<TriangleNode, uint32>* newNode = new AStarNode<TriangleNode, uint32>(neighbor, g, h);
				newNode->setCameFrom(x);
				visited.put(neighbor->getID(), newNode);
				priorityQueue.add(newNode);
			}
		}
	}

	priorityQueue.clearWithoutTraverse();

	if (goal == nullptr) {
		//cleanup all nodes
		visited.put(start->getID(), start);

		for (auto& val : visited) {
			delete val.getValue();
		}

		return nullptr;
	}

	auto path = new Vector<const Triangle*>();
	path->add(goal->getNode());

	const AStarNode<TriangleNode, uint32>* parent = goal->getCameFrom();

	while (parent != nullptr) {
		path->insertElementAt(parent->getNode(), 0);
		parent = parent->getCameFrom();
	}

	//cleanup all nodes
	visited.put(start->getID(), start);
	visited.put(goal->getID(), goal);

	for (auto& val : visited) {
		delete val.getValue();
	}

	return path;
}
