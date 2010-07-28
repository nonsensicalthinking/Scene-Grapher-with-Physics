/*
 * Camera.cpp
 *
 *	This will hold positional and directional data for this particular camera
 *	and will also hold its manipulator functions
 *
 *
 *  Created on: Jun 29, 2010
 *      Author: Derek Brooks
 */

#include "Camera.h"

Camera::Camera() {
	constDir[0] = 0.0;
	constDir[1] = 0.0;
	constDir[2] = 1.0;

	origin[0] = 0.0;
	origin[1] = 0.0;
	origin[2] = 0.0;

	dir[0] = 0.0;
	dir[1] = 0.0;
	dir[2] = 1.0;

	up[0] = 0.0;
	up[1] = 1.0;
	up[2] = 0.0;

	angles[0] = 0;
	angles[1] = 0;
	angles[2] = 0;

	pitch_rate = 0.22;
	yaw_rate = 0.22;
	roll_rate = 0.2;

	calcDirectionVector();
	lookAtDir();
}

Camera::~Camera() {
	// TODO Auto-generated destructor stub
}

void Camera::moveCameraLeft(float units)	{
	origin[0]-=units;
	lookAtDir();
}

void Camera::moveCameraRight(float units)	{
	origin[0]+=units;
	lookAtDir();
}

void Camera::moveCameraUp(float units)		{
	origin[1]+=units;
	lookAtDir();
}

void Camera::moveCameraDown(float units)	{
	origin[1]-=units;
	lookAtDir();
}

void Camera::moveCameraForward(float units)	{
	origin[2]+=units;
	lookAtDir();
}

void Camera::moveCameraBack(float units)	{
	origin[2]-=units;
	lookAtDir();
}

void Camera::rotateAboutX(float rad)	{
	angles[0] += rad;
	calcDirectionVector();
	lookAtDir();
}

void Camera::rotateAboutY(float rad)	{
	angles[1] += rad;
	calcDirectionVector();
	lookAtDir();
}

void Camera::rotateAboutZ(float rad)	{
	angles[2] += rad;
	calcDirectionVector();
	lookAtDir();
}

void Camera::lookAtDir()	{
	VectorAdd(origin, normDir, dir);
}

void Camera::calcDirectionVector()	{
	float rotMatrix[3][3];
	getRotationMatrix(angles, rotMatrix);

	float result[3];
	float o[3];
	VectorCopy(constDir, o);
	MatrixMultiply1x3(&o, rotMatrix, &normDir);
}


