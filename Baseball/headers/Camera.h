/*
 * Camera.h
 *
 *  Created on: Jun 29, 2010
 *      Author: Derek Brooks
 */
#include "shared.h"
#include "matrix.h"

#ifndef CAMERA_H_
#define CAMERA_H_

#define MOVESPEED	6

class Camera {
public:
	vec3_t origin;
	vec3_t dir;	// This is the camera direction
	vec3_t normDir;	// This is it normalized
	vec3_t constDir;// This is a constant direction North? sure. 0,0,1
	vec3_t up;
	vec3_t angles;	// angles are in radian measure

	float pitch_rate;
	float yaw_rate;
	float roll_rate;
	int fov;

	Camera();
	virtual ~Camera();

	void moveCameraForward(float units);
	void moveCameraBack(float units);
	void moveCameraDown(float units);
	void moveCameraUp(float units);
	void moveCameraRight(float units);
	void moveCameraLeft(float units);
	void rotateAboutX(float rad);	// Look Up/Down
	void rotateAboutY(float rad);	// Look Left/Right
	void rotateAboutZ(float rad);	// Look Roll (useful?)
	void calcDirectionVector();
	void lookAtDir();
	void goTo(vec3_t o);
	void setFieldOfView(int degrees);
};

#endif /* CAMERA_H_ */
