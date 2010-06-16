/*
 * scene.cpp
 *
 * The scene is the heart of the simulation / game.  This class handles
 * everything in the "world".  Makes calls to advance the simulation, stores
 * and manages everything in the scene, BSP Trees, Collision Detection...etc
 *
 *
 *  Created on: Feb 25, 2010
 *      Author: brof
 */
#include "scene.h"
#include "shared.h"
#include "physics.h"
#include <iostream>
#include <GL/glut.h>

#define MODEL 			"models/tallguy.md2"
#define FLOOR_RADIUS 	100

using namespace std;


const vec3_t GRAVITY_EARTH = {0.0f, -9.81f, 0.0f};


// Scene Globals
float timeElapsed 		= 0.0;
float slowMotionRatio 	= 1.0;


MotionUnderGravitation* motionUnderGravitation;
vec3_t startPos = {0.0, 0.0, 0.0};
vec3_t startAngle = {10.0, 15.0, 0.0};

Scene::Scene()
{
//	m = MD2Model::load(MODEL);

	// TODO REMOVE, This is just a test object for which to test the physics header
	vec3_t startVel;

	VectorSubtract(startPos, startAngle, startVel);
	float len = VectorLength(startVel);

	vec3_t heading;
	VectorSubtract(startAngle, startPos, heading);
	VectorUnitVector(heading, heading);

	cout << "Heading: [" << heading[0] << ", " << heading[1] << ", " << heading[2] << "] ";
	cout << "Speed: " << len << endl;

//	VectorScale( heading, len, startVel);
	VectorCopy(startAngle, startVel);

	motionUnderGravitation = new MotionUnderGravitation(GRAVITY_EARTH, startPos, startVel );
}


Scene::~Scene()
{
}


void Scene::render()
{
	int a;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// CYCLE ENTITIES AND DRAW WHAT WE NEED TO

	glPushMatrix();

	// Draw All Masses In motionUnderGravitation Simulation (Actually There Is Only One Mass In This Example Of Code)
	glColor3ub(255, 255, 0);									// Draw In Yellow

	for (a = 0; a < motionUnderGravitation->numOfMasses; ++a)
	{
		Mass* mass = motionUnderGravitation->getMass(a);

//		cout << mass->pos[0] << ", " << mass->pos[1] << ", " << mass->pos[2] << ", " << "Motion under gravitation\n";

		glPointSize(4);
		glBegin(GL_POINTS);
			glVertex3f(mass->pos[0], mass->pos[1], mass->pos[2]);
		glEnd();
	}
	glPopMatrix();

	glutSwapBuffers();	// swap out the display buffer with our new scene
}





void Scene::advance(long milliseconds)
{
	// Time work, used for Simulation work
	// dt Is The Time Interval (As Seconds) From The Previous Frame To The Current Frame.
	// dt Will Be Used To Iterate Simulation Values Such As Velocity And Position Of Masses.
	float dt = milliseconds / 1000.0f;							// Let's Convert Milliseconds To Seconds
	dt /= slowMotionRatio;										// Divide dt By slowMotionRatio And Obtain The New dt
	timeElapsed += dt;											// Iterate Elapsed Time
	float maxPossible_dt = 0.1f;								// Say That The Maximum Possible dt Is 0.1 Seconds
																// This Is Needed So We Do Not Pass Over A Non Precise dt Value
  	int numOfIterations = (int)(dt / maxPossible_dt) + 1;		// Calculate Number Of Iterations To Be Made At This Update Depending On maxPossible_dt And dt
	if (numOfIterations != 0)									// Avoid Division By Zero
		dt = dt / numOfIterations;								// dt Should Be Updated According To numOfIterations


	// Simulation work from here down

	for (int a = 0; a < numOfIterations; ++a)					// We Need To Iterate Simulations "numOfIterations" Times
	{
		motionUnderGravitation->operate(dt);					// Iterate motionUnderGravitation Simulation By dt Seconds
	}

}

// TODO REMOVE THIS TEST FUNCTION
void Scene::doItAgain()
{
	motionUnderGravitation = new MotionUnderGravitation(GRAVITY_EARTH, startPos, startAngle);
}



// Example of animating md2 model
/*
	glPushMatrix();

	glTranslated(-5.0, 10.0, 0.0);
	glRotated(-90.0, 0.0, 0.0, 1.0);
	glRotated(-45.0, 1.0, 0.0, 0.0);

	m->setAnimation("run");
	m->advance(0.01);
	m->draw();

	glPopMatrix();
*/











