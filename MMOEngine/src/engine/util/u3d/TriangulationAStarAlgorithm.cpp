/*
 * TriangulationAStarAlgorithm.cpp
 *
 *  Created on: 11/03/2011
 *      Author: victor
 */

//based on http://skatgame.net/mburo/ps/thesis_demyen_2006.pdf

#include "TriangulationAStarAlgorithm.h"
#include "Segment.h"

Vector<Triangle*>* TriangulationAStarAlgorithm::search(const Vector3& startPoint, const Vector3& goalPoint, TriangleNode* source, TriangleNode* target) {
	VectorMap<uint32, AStarNode<TriangleNode, uint32>* > visited(100, 10);
	visited.setNullValue(NULL);
	visited.setNoDuplicateInsertPlan();

	PriorityQueue priorityQueue;

	AStarNode<TriangleNode, uint32>* start = new AStarNode<TriangleNode, uint32>(source, 0, startPoint.distanceTo(goalPoint));
	priorityQueue.add(start);

	AStarNode<TriangleNode, uint32>* goal = NULL;

	while (!priorityQueue.isEmpty() && goal == NULL) {
		AStarNode<TriangleNode, uint32>* x = static_cast<AStarNode<TriangleNode, uint32>*>(const_cast<PriorityQueueEntry*>(priorityQueue.poll()));

		if (target == x->getNode()) {
			goal = x;
		} else {
			const Vector<TriangleNode*>* neighbors = x->getNode()->getNeighbors();

			for (int i = 0; i < neighbors->size(); ++i) {
				TriangleNode* neighbor = neighbors->getUnsafe(i);

				if (visited.contains(neighbor->getID()))
					continue;

				Triangle* triangleA = x->getNode();
				Triangle* triangleB = neighbor;

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

				while (parent != NULL) {
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

	priorityQueue.clear();

	if (goal == NULL) {
		//cleanup all nodes
		visited.put(start->getID(), start);

		for (auto& val : visited) {
			delete val.getValue();
		}

		return NULL;
	}

	Vector<Triangle*>* path = new Vector<Triangle*>();
	path->add(goal->getNode());

	AStarNode<TriangleNode, uint32>* parent = goal->getCameFrom();

	while (parent != NULL) {
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
