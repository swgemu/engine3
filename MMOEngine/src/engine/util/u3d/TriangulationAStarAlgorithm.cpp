/*
 * TriangulationAStarAlgorithm.cpp
 *
 *  Created on: 11/03/2011
 *      Author: victor
 */

//based on http://skatgame.net/mburo/ps/thesis_demyen_2006.pdf

#include "TriangulationAStarAlgorithm.h"
#include "Segment.h"
#include "Funnel.h"

Vector<Triangle*>* TriangulationAStarAlgorithm::search(const Vector3& startPoint, const Vector3& goalPoint, TriangleNode* source, TriangleNode* target) {
	HashTable<uint32, Reference<AStarNode<TriangleNode, uint32>* > > visited;
	visited.setNullValue(NULL);

	PriorityQueue priorityQueue;

	Reference<AStarNode<TriangleNode, uint32>* > start = new AStarNode<TriangleNode, uint32>(source, 0, startPoint.squaredDistanceTo(goalPoint));
	priorityQueue.add(start);

	Reference<AStarNode<TriangleNode, uint32>* > goal = NULL;

	while (priorityQueue.size() != 0 && goal == NULL) {
		Reference<AStarNode<TriangleNode, uint32>* > x = (AStarNode<TriangleNode, uint32>*)const_cast<PriorityQueueEntry*>(priorityQueue.poll());

		if (target == x->getNode()) {
			goal = x;
		} else {
			Vector<TriangleNode*>* neighbors = x->getNode()->getNeighbors();

			for (int i = 0; i < neighbors->size(); ++i) {
				TriangleNode* neighbor = neighbors->get(i);

				if (visited.containsKey(neighbor->getID()))
					continue;

				Triangle* triangleA = x->getNode();
				Triangle* triangleB = neighbor;

				Vector3 pointA, pointB;

				triangleA->getSharedVertices(triangleB, pointA, pointB);

				Vector3 closestPoint = Segment::getClosestPoint(pointA, pointB, goalPoint);

				float h = closestPoint.distanceTo(goalPoint);

				closestPoint = Segment::getClosestPoint(pointA, pointB, startPoint);

				float distEA = closestPoint.distanceTo(startPoint);

				//funnel distance
				//replacing with straight line for performance

				//funnel

				Vector<Triangle*> currentPath;
				currentPath.add(neighbor);
				//path->add(goal->getNode());

				AStarNode<TriangleNode, uint32>* parent = x;

				while (parent != NULL) {
					currentPath.insertElementAt(parent->getNode(), 0);
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

				//

			//	float estimatedPathDistance = neighbor->getBarycenter().squaredDistanceTo(startPoint);

				float g = MAX(MAX(x->getG(), distEA), funnelPathDistance);

				AStarNode<TriangleNode, uint32>* newNode = new AStarNode<TriangleNode, uint32>(neighbor, g, h);
				newNode->setCameFrom(x);
				visited.put(neighbor->getID(), newNode);
				priorityQueue.add(newNode);
			}
		}
	}

	if (goal == NULL)
		return NULL;

	Vector<Triangle*>* path = new Vector<Triangle*>();
	path->add(goal->getNode());

	AStarNode<TriangleNode, uint32>* parent = goal->getCameFrom();

	while (parent != NULL) {
		path->insertElementAt(parent->getNode(), 0);
		parent = parent->getCameFrom();
	}

	return path;
}
