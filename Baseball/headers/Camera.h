/*
 * Camera.h
 *
 *  Created on: Jun 29, 2010
 *      Author: Derek Brooks
 */
#include "shared.h"


#ifndef CAMERA_H_
#define CAMERA_H_

#define MOVESPEED	6

class Camera {



public:
	vec3_t origin;
	vec3_t dir;
	vec3_t normDir;
	vec3_t constDir;
	vec3_t up;
	vec3_t angles;	// angles are in radian measure

	float pitch_rate;
	float yaw_rate;
	float roll_rate;

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
};

#endif /* CAMERA_H_ */
