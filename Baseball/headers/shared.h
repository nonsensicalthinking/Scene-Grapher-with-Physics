/*
 * shared.h
 *
 *  Created on: Jun 4, 2010
 *      Author: brof
 */

#include <string>
#include <math.h>

using namespace std;

#ifndef SHARED_H_
#define SHARED_H_


typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];


typedef struct plane_s	{
	vec3_t origin;
	vec3_t normal;
	float length;	// optional, for limiting plane length
					// uses: bsp tree creation
}plane_t;


#define MAX_POLY_POINTS		10
typedef struct polygon_s	{
	int numPoints;
	vec3_t points[MAX_POLY_POINTS];
	// we can add:
	// - texture coords (uv st)
	// - texture id or name



	// for linking polygons
	polygon_s *nextPoly;
	polygon_s *prevPoly;

}polygon_t;

void VectorInit(vec3_t vec)	{
	vec[0] = 0;
	vec[1] = 0;
	vec[2] = 0;
}

void VectorCopy(const vec3_t a, vec3_t b)	{
	b[0] = a[0];
	b[1] = a[1];
	b[2] = a[2];
}

float DotProduct(const vec3_t a, const vec3_t b)	{
	return ( (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]) );
}

// TODO: Define the cross product
void CrossProduct(const vec3_t a, const vec3_t b, vec3_t result)	{
//	result[0] = a[0]*b[0];
//	result[1] = a[1]*b[1];
//	result[2] = a[2]*b[2];
}

// subtracts vec3_t b from vec3_t a
void VectorSubtract(const vec3_t a, const vec3_t b, vec3_t result)	{
	result[0] = a[0] - b[0];
	result[1] = a[1] - b[1];
	result[2] = a[2] - b[2];
}

// add both vectors together
void VectorAdd(const vec3_t a, const vec3_t b, vec3_t result)	{
	result[0] = a[0] + b[0];
	result[1] = a[1] + b[1];
	result[2] = a[2] + b[2];
}

void VectorScale(const vec3_t a, const float scale, vec3_t result)	{
	result[0] = a[0] * scale;
	result[1] = a[1] * scale;
	result[2] = a[2] * scale;
}

// VectorM(ultiply)A(dd) multiplies b * scale, then adds the new vec to a
void VectorMA(const vec3_t a, const vec3_t b, const float scale, vec3_t result)	{
	result[0] = a[0] + (b[0] * scale);
	result[1] = a[1] + (b[1] * scale);
	result[2] = a[2] + (b[2] * scale);
}

// FIXME: this seems costly
float VectorLength(const vec3_t a)	{
	return sqrtf( ((a[0]*a[0]) + (a[1]*a[1]) + (a[2]*a[2])) );
}

// FIXME: eeeee! Surely we can find a better way to perform this operation
void VectorDivide(const vec3_t a, const float divisor, vec3_t result)	{
	result[0] = a[0]/divisor;
	result[1] = a[1]/divisor;
	result[2] = a[2]/divisor;
}

void VectorUnitVector(const vec3_t a, vec3_t result)	{
	float length = VectorLength(a);

	result[0] = a[0]/length;
	result[1] = a[1]/length;
	result[2] = a[2]/length;
}

void VectorNegate(const vec3_t a, vec3_t result)	{
	result[0] = -a[0];
	result[1] = -a[1];
	result[2] = -a[2];
}

void VectorPrint(const vec3_t v)
{
	cout << "[" << v[0] << ", " << v[1] << ", " << v[2] << "]";
}



#endif /* SHARED_H_ */
