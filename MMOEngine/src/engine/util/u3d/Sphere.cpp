/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * Sphere.cpp
 *
 *  Created on: 11/03/2011
 *      Author: victor
 */

#include "Sphere.h"
#include "AABBTree.h"

bool Sphere::intersects(const AABB& aabb) const {
	float distSqrtToOrigin = aabb.distSqrd(origin);
	bool ret = ( distSqrtToOrigin <= radiusSquared);

	/*StringBuffer msg;

	if (ret)
		msg << "intersecting with mbox distance ";
	else
		msg << "not intersecting with mbox distance";

	msg << "distSqrtToOrigin:" << distSqrtToOrigin << " distNotSquared:" << sqrt(distSqrtToOrigin);
	msg << " radiusSquared:" << radiusSquared;

	Logger::console.info(msg.toString(), true);*/

	return ret;
}

bool Sphere::intersects(const Triangle& tri) const {
	// Early exit if one of the vertices is inside the sphere
	/*StringBuffer msg;
	msg << "checking against triangle:";
	msg << "vertex[0] x:" << tri.getVertex(0)[0] << " y:" << tri.getVertex(0)[1] << " z:" << tri.getVertex(0)[2] << endl;
	msg << "vertex[1] x:" << tri.getVertex(1)[0]<< " y:" << tri.getVertex(1)[1] << " z:" << tri.getVertex(1)[2];
	msg << "vertex[2] x:" << tri.getVertex(2)[0]<< " y:" << tri.getVertex(2)[1] << " z:" << tri.getVertex(2)[2];

	Logger::console.info(msg.toString(), true);*/

	float mRadius2 = radiusSquared;

	const Vector3& mCenter = origin;
	const Vector3& vert0 = tri.getVertex(0);
	const Vector3& vert1 = tri.getVertex(1);
	const Vector3& vert2 = tri.getVertex(2);

	Vector3 kDiff = vert2 - mCenter;

	float fC = kDiff.squaredLength();
	if (fC <= mRadius2)
		return true;

	kDiff = vert1 - mCenter;
	fC = kDiff.squaredLength();
	if (fC <= mRadius2)
		return true;

	kDiff = vert0 - mCenter;
	fC = kDiff.squaredLength();
	if(fC <= mRadius2)
		return true;

	// Else do the full distance test
	Vector3 TriEdge0 = vert1 - vert0;
	Vector3 TriEdge1 = vert2 - vert0;

	//Point kDiff	= vert0 - mCenter;
	float fA00	= TriEdge0.squaredLength();
	float fA01	= TriEdge0.dotProduct(TriEdge1);
	float fA11	= TriEdge1.squaredLength();
	float fB0	= kDiff.dotProduct(TriEdge0);
	float fB1	= kDiff.dotProduct(TriEdge1);
	//float fC	= kDiff.SquareMagnitude();
	float fDet	= fabsf(fA00 * fA11 - fA01* fA01);
	float u		= fA01 * fB1 - fA11 * fB0;
	float v		= fA01 * fB0 - fA00 * fB1;
	float SqrDist;

	if(u + v <= fDet)
	{
		if(u < 0.0f)
		{
			if(v < 0.0f)  // region 4
			{
				if(fB0 < 0.0f)
				{
					//					v = 0.0f;
					if(-fB0>=fA00)			{ /*u = 1.0f;*/		SqrDist = fA00+2.0f*fB0+fC;	}
					else					{ u = -fB0/fA00;	SqrDist = fB0*u+fC;			}
				}
				else
				{
					//					u = 0.0f;
					if(fB1>=0.0f)			{ /*v = 0.0f;*/		SqrDist = fC;				}
					else if(-fB1>=fA11)		{ /*v = 1.0f;*/		SqrDist = fA11+2.0f*fB1+fC;	}
					else					{ v = -fB1/fA11;	SqrDist = fB1*v+fC;			}
				}
			}
			else  // region 3
			{
				//				u = 0.0f;
				if(fB1>=0.0f)				{ /*v = 0.0f;*/		SqrDist = fC;				}
				else if(-fB1>=fA11)			{ /*v = 1.0f;*/		SqrDist = fA11+2.0f*fB1+fC;	}
				else						{ v = -fB1/fA11;	SqrDist = fB1*v+fC;			}
			}
		}
		else if(v < 0.0f)  // region 5
		{
			//			v = 0.0f;
			if(fB0>=0.0f)					{ /*u = 0.0f;*/		SqrDist = fC;				}
			else if(-fB0>=fA00)				{ /*u = 1.0f;*/		SqrDist = fA00+2.0f*fB0+fC;	}
			else							{ u = -fB0/fA00;	SqrDist = fB0*u+fC;			}
		}
		else  // region 0
		{
			// minimum at interior point
			if(fDet==0.0f)
			{
				//				u = 0.0f;
				//				v = 0.0f;
				SqrDist = MAX_FLOAT;
			}
			else
			{
				float fInvDet = 1.0f/fDet;
				u *= fInvDet;
				v *= fInvDet;
				SqrDist = u*(fA00*u+fA01*v+2.0f*fB0) + v*(fA01*u+fA11*v+2.0f*fB1)+fC;
			}
		}
	}
	else
	{
		float fTmp0, fTmp1, fNumer, fDenom;

		if(u < 0.0f)  // region 2
		{
			fTmp0 = fA01 + fB0;
			fTmp1 = fA11 + fB1;
			if(fTmp1 > fTmp0)
			{
				fNumer = fTmp1 - fTmp0;
				fDenom = fA00-2.0f*fA01+fA11;
				if(fNumer >= fDenom)
				{
					//					u = 1.0f;
					//					v = 0.0f;
					SqrDist = fA00+2.0f*fB0+fC;
				}
				else
				{
					u = fNumer/fDenom;
					v = 1.0f - u;
					SqrDist = u*(fA00*u+fA01*v+2.0f*fB0) + v*(fA01*u+fA11*v+2.0f*fB1)+fC;
				}
			}
			else
			{
				//				u = 0.0f;
				if(fTmp1 <= 0.0f)		{ /*v = 1.0f;*/		SqrDist = fA11+2.0f*fB1+fC;	}
				else if(fB1 >= 0.0f)	{ /*v = 0.0f;*/		SqrDist = fC;				}
				else					{ v = -fB1/fA11;	SqrDist = fB1*v+fC;			}
			}
		}
		else if(v < 0.0f)  // region 6
		{
			fTmp0 = fA01 + fB1;
			fTmp1 = fA00 + fB0;
			if(fTmp1 > fTmp0)
			{
				fNumer = fTmp1 - fTmp0;
				fDenom = fA00-2.0f*fA01+fA11;
				if(fNumer >= fDenom)
				{
					//					v = 1.0f;
					//					u = 0.0f;
					SqrDist = fA11+2.0f*fB1+fC;
				}
				else
				{
					v = fNumer/fDenom;
					u = 1.0f - v;
					SqrDist = u*(fA00*u+fA01*v+2.0f*fB0) + v*(fA01*u+fA11*v+2.0f*fB1)+fC;
				}
			}
			else
			{
				//				v = 0.0f;
				if(fTmp1 <= 0.0f)		{ /*u = 1.0f;*/		SqrDist = fA00+2.0f*fB0+fC;	}
				else if(fB0 >= 0.0f)	{ /*u = 0.0f;*/		SqrDist = fC;				}
				else					{ u = -fB0/fA00;	SqrDist = fB0*u+fC;			}
			}
		}
		else  // region 1
		{
			fNumer = fA11 + fB1 - fA01 - fB0;
			if(fNumer <= 0.0f)
			{
				//				u = 0.0f;
				//				v = 1.0f;
				SqrDist = fA11+2.0f*fB1+fC;
			}
			else
			{
				fDenom = fA00-2.0f*fA01+fA11;
				if(fNumer >= fDenom)
				{
					//					u = 1.0f;
					//					v = 0.0f;
					SqrDist = fA00+2.0f*fB0+fC;
				}
				else
				{
					u = fNumer/fDenom;
					v = 1.0f - u;
					SqrDist = u*(fA00*u+fA01*v+2.0f*fB0) + v*(fA01*u+fA11*v+2.0f*fB1)+fC;
				}
			}
		}
	}

	//Logger::console.info("Squared distance from triangle " + String::valueOf(fabsf(SqrDist)), true);

	return fabsf(SqrDist) < mRadius2;
}
