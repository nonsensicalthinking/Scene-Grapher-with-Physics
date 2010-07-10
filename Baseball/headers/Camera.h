/*
 * Camera.h
 *
 *  Created on: Jun 29, 2010
 *      Author: Derek Brooks
 */
#include "shared.h"


#ifndef CAMERA_H_
#define CAMERA_H_

class Camera {



public:
	vec3_t origin;
	vec3_t direction;
	vec3_t up;

	Camera();
	virtual ~Camera();

	void moveCameraForward(double units);
	void moveCameraBack(double units);
	void moveCameraDown(double units);
	void moveCameraUp(double units);
	void moveCameraRight(double units);
	void moveCameraLeft(double units);



};

#endif /* CAMERA_H_ */
