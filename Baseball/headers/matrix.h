/*
 * matrix.h
 *
 *  Created on: Jul 29, 2010
 *      Author: brof
 */

#include <math.h>

#ifndef MATRIX_H_
#define MATRIX_H_


typedef float matrix_cell_t;
typedef matrix_cell_t matrix_3x3_t[3][3];
typedef matrix_cell_t matrix_1x3_t[3];


inline void MatrixMultiply3x3(matrix_3x3_t matrixA, matrix_3x3_t matrixB, matrix_3x3_t result) {
	result[0][0] = 	matrixA[0][0] * matrixB[0][0] +
					matrixA[0][1] * matrixB[1][0] +
					matrixA[0][2] * matrixB[2][0];

	result[0][1] = 	matrixA[0][0] * matrixB[0][1] +
					matrixA[0][1] * matrixB[1][1] +
					matrixA[0][2] * matrixB[2][1];

	result[0][2] = 	matrixA[0][0] * matrixB[0][2] +
					matrixA[0][1] * matrixB[1][2] +
					matrixA[0][2] * matrixB[2][2];

	result[1][0] = 	matrixA[1][0] * matrixB[0][0] +
					matrixA[1][1] * matrixB[1][0] +
					matrixA[1][2] * matrixB[2][0];

	result[1][1] = 	matrixA[1][0] * matrixB[0][1] +
					matrixA[1][1] * matrixB[1][1] +
					matrixA[1][2] * matrixB[2][1];

	result[1][2] = 	matrixA[1][0] * matrixB[0][2] +
					matrixA[1][1] * matrixB[1][2] +
					matrixA[1][2] * matrixB[2][2];

	result[2][0] = 	matrixA[2][0] * matrixB[0][0] +
					matrixA[2][1] * matrixB[1][0] +
					matrixA[2][2] * matrixB[2][0];

	result[2][1] = 	matrixA[2][0] * matrixB[0][1] +
					matrixA[2][1] * matrixB[1][1] +
					matrixA[2][2] * matrixB[2][1];

	result[2][2] = 	matrixA[2][0] * matrixB[0][2] +
					matrixA[2][1] * matrixB[1][2] +
					matrixA[2][2] * matrixB[2][2];
}

inline void MatrixMultiply1x3(matrix_3x3_t matrixA, matrix_3x3_t matrixB, matrix_3x3_t result) {
	result[0][0] = 	matrixA[0][0] * matrixB[0][0] +
					matrixA[0][1] * matrixB[1][0] +
					matrixA[0][2] * matrixB[2][0];

	result[0][1] = 	matrixA[0][0] * matrixB[0][1] +
					matrixA[0][1] * matrixB[1][1] +
					matrixA[0][2] * matrixB[2][1];

	result[0][2] = 	matrixA[0][0] * matrixB[0][2] +
					matrixA[0][1] * matrixB[1][2] +
					matrixA[0][2] * matrixB[2][2];
}

inline void XRotationMatrix(float xRad, matrix_3x3_t result)	{
	result[0][0] = 1;
	result[0][1] = 0;
	result[0][2] = 0;

	result[1][0] = 0;
	result[1][1] = cos(xRad);
	result[1][2] = -sin(xRad);

	result[2][0] = 0;
	result[2][1] = sin(xRad);
	result[2][2] = cos(xRad);
}

inline void YRotationMatrix(float yRad, matrix_3x3_t result)	{
	result[0][0] = cos(yRad);
	result[0][1] = 0;
	result[0][2] = sin(yRad);

	result[1][0] = 0;
	result[1][1] = 1;
	result[1][2] = 0;

	result[2][0] = -sin(yRad);
	result[2][1] = 0;
	result[2][2] = cos(yRad);
}

inline void ZRotationMatrix(float zRad, matrix_3x3_t result)	{
	result[0][0] = cos(zRad);
	result[0][1] = -sin(zRad);
	result[0][2] = 0;

	result[1][0] = sin(zRad);
	result[1][1] = cos(zRad);
	result[1][2] = 0;

	result[2][0] = 0;
	result[2][1] = 0;
	result[2][2] = 1;
}


// rot is a 1x3 matrix where you've entered x, y and z rotation
// quantified by degrees of movement, you get back the proper rotation matrix
inline void getRotationMatrix(matrix_1x3_t rot, matrix_3x3_t result)	{
	matrix_3x3_t xrot, yrot, zrot;

	XRotationMatrix(rot[0], xrot);	// Pitch
	YRotationMatrix(rot[1], yrot);	// Yaw
	ZRotationMatrix(rot[2], zrot);	// Roll

	MatrixMultiply3x3(xrot, yrot, result);
	MatrixMultiply3x3(result, zrot, result);
}

#endif /* MATRIX_H_ */
