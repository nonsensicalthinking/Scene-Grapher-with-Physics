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
#include <GL/glut.h>

#define SCENE_W	800
#define SCENE_H	600


Camera::Camera() {
	constDir[0] = 0.0;
	constDir[1] = 0.0;
	constDir[2] = 1.0;

	origin[0] = -42.0;
	origin[1] = 4.0;
	origin[2] = 36.0;

	dir[0] = 0.0;
	dir[1] = 0.0;
	dir[2] = 0.0;

	up[0] = 0.0;
	up[1] = 1.0;
	up[2] = 0.0;

	angles[0] = 0;
	angles[1] = degToRad(-130);
	angles[2] = 0;

	pitch_rate = 0.22;
	yaw_rate = 0.22;
	roll_rate = 0.2;

	calcDirectionVector();
	lookAtDir();
	setFieldOfView(90);
}

Camera::~Camera() {
	// nothing to destruct
}

void Camera::moveCameraRight(float units)	{
	vec3_t r;
	CrossProduct(up, normDir, r);
	VectorNegate(r, r);
	VectorScale(r, units, r);
	VectorAdd(origin, r, origin);
	lookAtDir();
}

void Camera::moveCameraLeft(float units)	{
	vec3_t r;
	CrossProduct(up, normDir, r);
	VectorScale(r, units, r);
	VectorAdd(origin, r, origin);
	lookAtDir();
}

void Camera::moveCameraForward(float units)	{
	vec3_t r;
	VectorScale(normDir, units, r);
	VectorAdd(origin, r, origin);
	lookAtDir();
}

void Camera::moveCameraBack(float units)	{
	vec3_t backdir;
	VectorNegate(normDir, backdir);
	VectorScale(backdir, units, backdir);
	VectorAdd(origin, backdir, origin);
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

void Camera::goTo(vec3_t o)	{
	VectorCopy(o, origin);
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

void Camera::setFieldOfView(int degrees)	{
	fov = degrees;
	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the correct perspective.
	gluPerspective(fov, SCENE_W/SCENE_H, 1, 1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

