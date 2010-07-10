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

/*
 * TODO LIST for this file
 *
 * TODO: Generate display lists for polygons
 *
 *
 */




#include <iostream>
#include "Scene.h"
#include "shared.h"	// this is included in physics.h too
#include "physics.h"
#include "bsptree.h"
#include "font.h"
#include "Console.h"
#include "keys.h"
#include "strtools.h"
#include "objloader.h"	// TODO REMOVE THIS INCLUDE
#include <sstream>
#include <GL/glut.h>

Font* f;

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

// End Globals


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


Scene::Scene(int width, int height)
{
	sceneWidth = width;
	sceneHeight = height;

	consoleActive = false;
	con = new Console(width,height);

	polygonList = new list<polygon_t*>;
	matsManager = getTextureManager();

	cam = new Camera();

	// temp frame rate display
	f = new Font(sceneWidth, sceneHeight);


	//	m = MD2Model::load(MODEL);


//	bspRoot = new bsp_node_t;
//	createSimpleBSP(bspRoot);


	for(int x=0; x < 20; x++)	{
		ostringstream s;
		s << "this is line #" << x << "\n";
		con->output->push_back(s.str());
	}

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
	delete con;
}


void Scene::LoadMap(string map)	{
	ObjModel* obj = new ObjModel();
	obj->loadObjFile("fenway.obj");
}


// TODO Test me
// This should be used when "Loading the map"
void glCachePolygon(polygon_t* polygon)	{
    polygon->glCacheID = glGenLists(1);
    glNewList(polygon->glCacheID, GL_COMPILE);
//    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glBegin(GL_POLYGON);
    for(int loop = 0; loop < polygon->numPoints ; loop++)	{
//		glTexCoord2f(u_coord, v_coord);
		glVertex3f(polygon->points[loop][0], polygon->points[loop][1], polygon->points[loop][2]);
    }
    glEnd();
	glEndList();
}

void Scene::addPolygon(polygon_t* p)	{
	polygonList->push_back(p);
}

void Scene::drawPolygon(polygon_t* poly)	{
	glPushMatrix();

		if( poly->hasMaterial )
			matsManager->enableMaterial(poly->materialName);

		glBegin(GL_POLYGON);
		for(int x=0; x < poly->numPoints; x++)	{
			if( poly->hasNormals )
				glNormal3f(poly->normpts[x][0], poly->normpts[x][1], poly->normpts[x][2] );

			if( poly->isTextured )
				glTexCoord2f(poly->texpts[x][0], poly->texpts[x][1]);

			glVertex3f(poly->points[x][0], poly->points[x][1], poly->points[x][2]);
		}
		glEnd();

		if( poly->hasMaterial )
			matsManager->disableMaterial(poly->materialName);

	glPopMatrix();
}

void Scene::renderPolygonList(list<polygon_t*> polygons)
{
	list<polygon_t*>::iterator itr;

	// for each polygon in the list
	for(itr = polygons.begin(); itr != polygons.end(); itr++)	{
		drawPolygon((*itr));
	}
}


/*
void renderBSPTree(bsp_node_t* tree)	{
	if( tree->isLeaf() )	{
		renderPolygonList(tree->getPolygonList());
	}
	else	{
		// perform render back to front
		renderBSPTree(tree->back);
		renderBSPTree(tree->front);
	}
}

*/

void Scene::performLighting()	{
	GLfloat spec[]={1.0, 1.0 ,1.0 ,1.0};      //sets specular highlight of balls
	GLfloat posl[]={0,400,0,1};               //position of ligth source
	GLfloat amb[]={0.2f, 0.2f, 0.2f ,1.0f};   //global ambient
	GLfloat amb2[]={0.3f, 0.3f, 0.3f ,1.0f};  //ambient of lightsource
	GLfloat df = 100.0;

	glMaterialfv(GL_FRONT,GL_SPECULAR,spec);
	glMaterialfv(GL_FRONT,GL_SHININESS,&df);

	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0,GL_POSITION,posl);
	glLightfv(GL_LIGHT0,GL_AMBIENT,amb2);
	glEnable(GL_LIGHT0);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,amb);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
}




void Scene::render()
{
	int a;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	performLighting();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// position camera
	gluLookAt(	cam->origin[0], cam->origin[1], cam->origin[2],	// camera origin
				cam->direction[0], cam->direction[1], cam->direction[2],		// eye looking @ this vertex
				cam->up[0], cam->up[1], cam->up[2]);	// up direction

	// End stuff to put into the scene

	// Advance scene's physical positioning and draw it
	// CYCLE ENTITIES AND DRAW WHAT WE NEED TO
//	if( bspRoot )	{
//		renderBSPTree(bspRoot);
//	}

	renderPolygonList(*polygonList);

	glPushMatrix();

	// Draw All Masses In motionUnderGravitation Simulation (Actually There Is Only One Mass In This Example Of Code)
//	glColor3ub(255, 255, 0);									// Draw In Yellow

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



	// Draw the console if open
	if( consoleActive )
		con->Draw();
	stringstream s;
	s << "FPS: " << getFrameRate();
	f->glPrint(0, 0, s.str().c_str(), 0);

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

#define CAM_MOVE_RATE	20.0

// Handles keyboard input from normal text keys
void Scene::keyPressed(unsigned char key)	{
//	cout << "Key pressed: " << key << endl;
	if( consoleActive )	{	// send key input to console
		switch(key)	{
			case CONSOLE_KEY:	// deactivate console
				consoleActive = !consoleActive;
				break;
			default:	// add to input line
				con->appendToInput(key);
		}
	}
	else	{	// Don't send key input to console
		switch(key)	{
			case '`':	// active console
				consoleActive = !consoleActive;
				break;
			case 'a':	// omg our first control over the scene!
				cam->moveCameraLeft(CAM_MOVE_RATE);
				break;
			case 'd':	// omg our first control over the scene!
				cam->moveCameraRight(CAM_MOVE_RATE);
				break;
			case 'w':	// omg our first control over the scene!
				cam->moveCameraForward(CAM_MOVE_RATE);
				break;
			case 's':	// omg our first control over the scene!
				cam->moveCameraBack(CAM_MOVE_RATE);
				break;
			case 'q':	// omg our first control over the scene!
				cam->moveCameraUp(CAM_MOVE_RATE);
				break;
			case 'e':	// omg our first control over the scene!
				cam->moveCameraDown(CAM_MOVE_RATE);
				break;
			case 'z':
				doItAgain();
				break;
		}
	}
}



// handles keyboard input from special keys
void Scene::specialKeyPressed(int key, int x, int y)	{
//	cout << "Special Key Pressed: " << key << endl;

	if( consoleActive )	{	// send key input to console
		switch(key)	{
			case F1_KEY:
				break;
			case F2_KEY:
				break;
			case F3_KEY:
				break;
			case F4_KEY:
				break;
			case F5_KEY:
				break;
			case F6_KEY:
				break;
			case F7_KEY:
				break;
			case F8_KEY:
				break;
			case F9_KEY:
				break;
			case F10_KEY:
				break;
			case F11_KEY:
				break;
			case F12_KEY:
				break;
			case ARROW_UP_KEY:
				break;
			case ARROW_DOWN_KEY:
				break;
			case PAGE_UP_KEY:
				con->scrollUp();
				break;
			case PAGE_DOWN_KEY:
				con->scrollDown();
				break;
			case HOME_KEY:
				break;
			case END_KEY:
				break;
			case INSERT_KEY:
				break;
			case NUM_LOCK_KEY:
				break;

		}
	}
}


void Scene::exit()	{
	cleanExit();
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




// TODO REMOVE THIS TEST FUNCTION
void Scene::doItAgain()
{
	motionUnderGravitation = new MotionUnderGravitation(GRAVITY_EARTH, startPos, startAngle);
}






