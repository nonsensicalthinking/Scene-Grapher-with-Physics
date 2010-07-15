/*
 * shared.h
 *
 *  Created on: Jun 4, 2010
 *      Author: Derek Brooks
 */


#include <iostream>
#include <string>
#include <math.h>

using namespace std;

#ifndef SHARED_H_
#define SHARED_H_

#define	BACK				-1
#define	SPANNING			0
#define	FRONT				1

#define EPSILON				0.0001f


#define MAX_POLY_POINTS		10		// Max number of points in a polygon
#define MAX_FILE_LENGTH		28		// Max length for a filename


typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];

const vec3_t NORMAL_X = {1.0, 0.0, 0.0};
const vec3_t NORMAL_Y = {0.0, 1.0, 0.0};
const vec3_t NORMAL_Z = {0.0, 0.0, 1.0};


typedef struct plane_s	{
	vec3_t origin;
	vec3_t normal;
}plane_t;



typedef struct polygon_s	{

	int numPoints;
	vec3_t points[MAX_POLY_POINTS];

	// TODO When the BSP is partitioning polygons, we
	// need to resize the texpts along with it, normals
	// will stay the same, but texpts needs recalculation.
	bool isTextured;
	vec2_t texpts[MAX_POLY_POINTS];

	bool hasNormals;
	vec3_t normpts[MAX_POLY_POINTS];

	int glCacheID;

	bool hasMaterial;
	char materialName[MAX_FILE_LENGTH];
}polygon_t;




inline void VectorInit(vec3_t vec)	{
	vec[0] = 0;
	vec[1] = 0;
	vec[2] = 0;
}

inline void VectorCopy(const vec3_t a, vec3_t b)	{
	b[0] = a[0];
	b[1] = a[1];
	b[2] = a[2];
}

inline float DotProduct(const vec3_t a, const vec3_t b)	{
	return ( (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]) );
}

// TODO: Define the cross product
inline void CrossProduct(const vec3_t a, const vec3_t b, vec3_t result)	{
//	result[0] = a[0]*b[0];
//	result[1] = a[1]*b[1];
//	result[2] = a[2]*b[2];
}

// subtracts vec3_t b from vec3_t a
inline void VectorSubtract(const vec3_t a, const vec3_t b, vec3_t result)	{
	result[0] = a[0] - b[0];
	result[1] = a[1] - b[1];
	result[2] = a[2] - b[2];
}

inline void VectorSubtract2f(const vec2_t a, const vec2_t b, vec2_t result)	{
	result[0] = a[0] - b[0];
	result[1] = a[1] - b[1];
}

inline void VectorAdd(const vec3_t a, const vec3_t b, vec3_t result)	{
	result[0] = a[0] + b[0];
	result[1] = a[1] + b[1];
	result[2] = a[2] + b[2];
}

inline void VectorScale(const vec3_t a, const float scale, vec3_t result)	{
	result[0] = a[0] * scale;
	result[1] = a[1] * scale;
	result[2] = a[2] * scale;
}

inline void VectorScale2f(const vec2_t a, const float scale, vec2_t result)	{
	result[0] = a[0] * scale;
	result[1] = a[1] * scale;
}

inline void VectorDivide(const vec3_t a, const float divisor, vec3_t result)	{
	result[0] = a[0] / divisor;
	result[1] = a[1] / divisor;
	result[2] = a[2] / divisor;
}

// multiplies b * scale, then adds the new vec to a
inline void VectorMA(const vec3_t a, const vec3_t b, const float scale, vec3_t result)	{
	result[0] = a[0] + (b[0] * scale);
	result[1] = a[1] + (b[1] * scale);
	result[2] = a[2] + (b[2] * scale);
}

inline void VectorMA2f(const vec2_t a, const vec2_t b, const float scale, vec2_t result)	{
	result[0] = a[0] + (b[0] * scale);
	result[1] = a[1] + (b[1] * scale);
}

// FIXME: this seems costly
inline float VectorLength(const vec3_t a)	{
	return sqrtf( ((a[0]*a[0]) + (a[1]*a[1]) + (a[2]*a[2])) );
}


inline void VectorUnitVector(const vec3_t a, vec3_t result)	{
	float length = VectorLength(a);

	VectorDivide(a, length, result);
}

inline void VectorNegate(const vec3_t a, vec3_t result)	{
	result[0] = -a[0];
	result[1] = -a[1];
	result[2] = -a[2];
}

inline void VectorPrint(const vec3_t v)	{
	cout << "[" << v[0] << ", " << v[1] << ", " << v[2] << "]";
}

inline float classifyPoint(const plane_t *plane, const vec3_t point)	{
	return DotProduct(plane->normal, point) - DotProduct(plane->normal, plane->origin);
}

inline int classifyPolygon(const plane_t* partition, const polygon_t* poly)	{
	int x;
	bool hasFront = false;
	bool hasBack = false;


	for(x=0; x < poly->numPoints; x++)	{
		float classification = classifyPoint(partition, poly->points[x]);

		// we can do the returns below because if any point on the
		// polygon is on the opposite side there will be a split
		// we don't care to check every point, the splitting routines
		// will do that for us later.
		if( classification  >= 0 )	{
			hasFront = true;

			if( hasBack )
				return SPANNING;

		}
		else	{ // if( classification < 0 )
			hasBack = true;

			if( hasFront )
				return SPANNING;
		}
	}

	if( hasFront )
		return FRONT;

	if( hasBack )
		return BACK;

	return -99;	// Error of sorts happened.
}

inline int findLinePlaneIntersect(const plane_t *plane, const vec3_t pointA, const vec3_t pointB, vec3_t intersect, vec3_t fractSect)	{
	vec3_t u;
	vec3_t w;

	VectorSubtract(pointB, pointA, u);
	VectorSubtract(pointA, plane->origin, w);

	float numerator = -DotProduct(plane->normal, w);
	float denominator = DotProduct(plane->normal, u);

	if( fabs(denominator) < EPSILON ) {          // segment is parallel to plane
		if (numerator == 0)                     // segment lies in plane
			return 2;
		else
			return 0;                   // no intersection
	}

	// they are not parallel
	// compute intersect param
	fractSect[0] = numerator / denominator;

	if( fractSect[0] < 0 || fractSect[0] > 1 )
		return 0;                       // no intersection

	VectorMA( pointA, u, fractSect[0], intersect);

	return 1;	// Indicate that we had an intersection
}







#endif /* SHARED_H_ */
