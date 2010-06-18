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
#include <iostream>
#include "scene.h"
#include "shared.h"	// this is included in physics.h too
#include "physics.h"
#include "BSPTree.h"
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


bsp_node_t* bspRoot;


void createSimpleBSP(bsp_node_t* root)	{
	list<polygon_t*> polygonList;

	polygon_t* p = new polygon_t;
	p->numPoints = 4;

	p->points[0][0] = -200;
	p->points[0][1] = 0;
	p->points[0][2] = -200;

	p->points[1][0] = -200;
	p->points[1][1] = 0;
	p->points[1][2] = 200;

	p->points[2][0] = 200;
	p->points[2][1] = 0;
	p->points[2][2] = 200;

	p->points[3][0] = 200;
	p->points[3][1] = 0;
	p->points[3][2] = -200;

	polygonList.push_back(p);

	plane_t* partition = new plane_t;

	partition->normal[0] = 1.0;
	partition->normal[1] = 0.0;
	partition->normal[2] = 0.0;

	partition->origin[0] = 0.0;
	partition->origin[1] = 0.0;
	partition->origin[2] = 0.0;

	root->setPolygonList(polygonList);


	buildTree(400, partition, root);
}




Scene::Scene()
{
//	m = MD2Model::load(MODEL);

	// TODO REMOVE, This is just a test object for which to test the physics header
	vec3_t startVel;

	bspRoot = new bsp_node_t;

	createSimpleBSP(bspRoot);


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






void renderPolygonList(list<polygon_t*> polygons)
{
	list<polygon_t*>::iterator itr;

	// for each polygon in the list
	for(itr = polygons.begin(); itr != polygons.end(); itr++)	{
		glColor3f(0.0, 0.0, 1.0);
		glPushMatrix();
		glBegin(GL_POLYGON);
		for(int x=0; x < (*itr)->numPoints; x++)	{
			vec3_t point;
			VectorCopy((*itr)->points[x], point);
			glVertex3f(point[0], point[1], point[2]);
		}
		glEnd();
		glPopMatrix();

	}
}



void renderBSPTree(bsp_node_t* tree)	{
	static int leafCount = 0;

	if( tree->isLeaf() )	{
		leafCount++;
		cout << "Rendering Leaf #" << leafCount << endl;
		renderPolygonList(tree->getPolygonList());
	}
	else	{
		// perform render back to front
		renderBSPTree(tree->back);
		renderBSPTree(tree->front);
	}
}




void Scene::render()
{
	int a;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// CYCLE ENTITIES AND DRAW WHAT WE NEED TO
	if( bspRoot )	{
		renderBSPTree(bspRoot);
	}


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











