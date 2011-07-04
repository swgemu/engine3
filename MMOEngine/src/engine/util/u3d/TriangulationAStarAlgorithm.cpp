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
	VectorMap<uint32, Reference<AStarNode<TriangleNode, uint32>* > > visited;
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

				if (visited.contains(neighbor->getID()))
					continue;

				Triangle* triangleA = x->getNode();
				Triangle* triangleB = neighbor;

				Vector3 pointA, pointB;

				triangleA->getSharedVertices(triangleB, pointA, pointB);

				Vector3 closestPoint = Segment::getClosestPoint(pointA, pointB, goalPoint);

				float h = closestPoint.squaredDistanceTo(goalPoint);

				closestPoint = Segment::getClosestPoint(pointA, pointB, startPoint);

				float distEA = closestPoint.squaredDistanceTo(startPoint);

				//funnel distance
				//replacing with straight line for performance

				float estimatedPathDistance = neighbor->getBarycenter().squaredDistanceTo(startPoint);

				float g = MAX(MAX(x->getG(), distEA), estimatedPathDistance);

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
