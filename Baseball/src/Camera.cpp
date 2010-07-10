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
	origin[0] = 0.0;
	origin[1] = 6.0;
	origin[2] = 0.0;

	direction[0] = 0.0;
	direction[1] = 6.0;
	direction[2] = -5.0;

	up[0] = 0.0;
	up[1] = 1.0;
	up[2] = 0.0;
}

Camera::~Camera() {
	// TODO Auto-generated destructor stub
}

void Camera::moveCameraLeft(double units)	{
	origin[0]-=units;
}

void Camera::moveCameraRight(double units)	{
	origin[0]+=units;
}

void Camera::moveCameraUp(double units)		{
	origin[1]+=units;
}

void Camera::moveCameraDown(double units)	{
	origin[1]-=units;
}

void Camera::moveCameraForward(double units)	{
	origin[2]+=units;
}

void Camera::moveCameraBack(double units)	{
	origin[2]-=units;
}

