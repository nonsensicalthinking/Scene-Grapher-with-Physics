/*
 * scene.cpp
 *
 * The scene is the heart of the simulation / game.  This class handles
 * everything in the "world".  Makes calls to advance the simulation, stores
 * and manages everything in the scene, BSP Trees, Collision Detection...etc
 *
 *
 *  Created on: Feb 25, 2010
 *      Author: Derek Brooks
 */

/*
 * TODO LIST for this file
 *
 * Empty for now, certainly not done with this file yet though.
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
#include "objloader.h"
#include <sstream>
#include <GL/glut.h>


#define MODEL 			"models/tallguy.md2"
#define CAM_MOVE_RATE	20.0
const vec3_t GRAVITY_EARTH = {0.0f, -9.81f, 0.0f};

using namespace std;

// Scene Globals
float timeElapsed 		= 0.0;
float slowMotionRatio 	= 1.0;
vec3_t startPos = {0.0, 0.0, 0.0};
vec3_t startAngle = {10.0, 15.0, 0.0};
MotionUnderGravitation* motionUnderGravitation;
bsp_node_t* bspRoot;
Font* f;

// End Globals

// Things are things that need to be done when
// "loading a map"
void Scene::createBSP()	{
	LoadMap("fenway.obj");
	bspRoot = new bsp_node_t;
	generateBSP(bspRoot);
	namePolygons(bspRoot);
	buildPolygonByNameMap(bspRoot);
}

void Scene::generateBSP(bsp_node_t* root)	{

	plane_t* partition = new plane_t;

	partition->normal[0] = 1.0;
	partition->normal[1] = 0.0;
	partition->normal[2] = 0.0;

	partition->origin[0] = 0.0;
	partition->origin[1] = 0.0;
	partition->origin[2] = 0.0;

	root->setPolygonList(*polygonList);

	// the first argument must be double the width of the largest surface
	buildTree(400, 0, partition, root);
}

void Scene::buildPolygonByNameMap(bsp_node_t* bspRootNode)	{
	if( bspRootNode->isLeaf() )	{
		list<polygon_t*>::iterator itr;
		for(itr = bspRootNode->beginPolyListItr(); itr != bspRootNode->endPolyListItr(); itr++)
			polygonByName[(*itr)->polyID] = (*itr);
	}
	else	{
		buildPolygonByNameMap(bspRootNode->front);
		buildPolygonByNameMap(bspRootNode->back);
	}
}

// DO THIS AND CACHE THE POLYGON TOO
void Scene::namePolygons(bsp_node_t* bspNode)	{
	list<polygon_t*>::iterator itr;

	if( bspNode->isLeaf() )	{
		for(itr = bspNode->beginPolyListItr(); itr != bspNode->endPolyListItr(); itr++)	{
			(*itr)->selected = false;
			(*itr)->polyID = polygonCount++;
			glCachePolygon(*itr);
		}
	}
	else	{
		namePolygons(bspNode->front);
		namePolygons(bspNode->back);
	}
}

void Scene::LoadMap(string map)	{
	ObjModel* obj = new ObjModel();
	obj->loadObjFile("fenway.obj");
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
	polygonCount = 0;	// For naming polygons
	isPicking = false;	// Picking flag, is temporary
	f = new Font(sceneWidth, sceneHeight);	// temp frame rate display


	//	m = MD2Model::load(MODEL);


	// TODO REMOVE, This is just a test object for which to test the physics header
	vec3_t startVel;
	VectorSubtract(startPos, startAngle, startVel);
	float len = VectorLength(startVel);

	vec3_t heading;
	VectorSubtract(startAngle, startPos, heading);
	VectorUnitVector(heading, heading);

//	cout << "Heading: [" << heading[0] << ", " << heading[1] << ", " << heading[2] << "] ";
//	cout << "Speed: " << len << endl;

//	VectorScale( heading, len, startVel);
	VectorCopy(startAngle, startVel);

	motionUnderGravitation = new MotionUnderGravitation(GRAVITY_EARTH, startPos, startVel );
	// END TODO REMOVE
}


Scene::~Scene()
{
	delete con;
}

void Scene::resizeSceneSize(int width, int height)	{
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(height == 0)
		height = 1;

	float ratio = 1.0* width / height;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
    glViewport(0, 0, width, height);

	// Set the correct perspective.
	gluPerspective(45,ratio,1,1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(	cam->origin[0], cam->origin[1], cam->origin[2],	// camera origin
				cam->direction[0], cam->direction[1], cam->direction[2],		// eye looking @ this vertex
				cam->up[0], cam->up[1], cam->up[2]);	// up direction
}


void Scene::performLighting()	{
	GLfloat spec[]={1.0, 1.0 ,1.0 ,1.0};      //sets specular highlight
	GLfloat posl[]={0,700,0,0};               //position of light source
	GLfloat amb[]={0.8f, 0.8f, 0.8f ,1.0f};   //global ambient
	GLfloat amb2[]={0.8f, 0.8f, 0.8f ,1.0f};  //ambiance of light source
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


void Scene::addPolygon(polygon_t* p)	{
	polygonList->push_back(p);
}


void Scene::glCachePolygon(polygon_t* polygon)	{
    polygon->glCacheID = glGenLists(1);
    glNewList(polygon->glCacheID, GL_COMPILE);
    drawPolygon(polygon, false);
    glEndList();
    polygon->glCached = true;
}


void Scene::drawPolygon(polygon_t* poly, bool selectMode)	{
	if( poly->glCached )	{
// try it out if you don't believe me...
//		cout << "Rendering cached polygons" << endl;
		if( selectMode )
			glPushName(poly->polyID);

		glCallList(poly->glCacheID);

		if( selectMode )
			glPopName();
	}
	else	{
		if( selectMode )
			glPushName(poly->polyID);

		glPushMatrix();
			if( selectMode && poly->selected )	{
				vec3_t Ka = {0,0,1};
				vec3_t Kd = {0,0,1};
				vec3_t Ks = {0,0,1};
				int Ns = 0;

				glMaterialfv(GL_FRONT, GL_AMBIENT, Ka);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, Kd);
				glMaterialfv(GL_FRONT, GL_SPECULAR, Ks);
				glMaterialf(GL_FRONT, GL_SHININESS, Ns);
			}
			else	{
				if( poly->hasMaterial )
					matsManager->enableMaterial(poly->materialName);
			}

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

		if( selectMode )
			glPopName();
	}
}

void Scene::renderPolygonList(list<polygon_t*> polygons, bool selectionMode)
{
	list<polygon_t*>::iterator itr;

	// for each polygon in the list
	for(itr = polygons.begin(); itr != polygons.end(); itr++)	{
		drawPolygon((*itr), selectionMode);
	}
}


void Scene::renderBSPTree(bsp_node_t* tree)	{
	if( tree->isLeaf() )	{
		renderPolygonList(tree->getPolygonList(), isPicking);
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


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	performLighting();

	gluLookAt(	cam->origin[0], cam->origin[1], cam->origin[2],	// camera origin
				cam->direction[0], cam->direction[1], cam->direction[2],		// eye looking @ this vertex
				cam->up[0], cam->up[1], cam->up[2]);	// up direction



	// Advance scene's physical positioning and draw it
	// CYCLE ENTITIES AND DRAW WHAT WE NEED TO
	if( bspRoot )	{
		renderBSPTree(bspRoot);
	}

	glPushMatrix();
		// Draw All Masses In motionUnderGravitation Simulation (Actually There Is Only One Mass In This Example Of Code)
		glColor3f(255, 255, 0);									// Draw In Yellow

		for (a = 0; a < motionUnderGravitation->numOfMasses; ++a)
		{
			Mass* mass = motionUnderGravitation->getMass(a);

			glPointSize(4);
			glBegin(GL_POINTS);
				glVertex3f(mass->pos[0], mass->pos[1], mass->pos[2]);
			glEnd();
		}
	glPopMatrix();


	// Disable lighting for drawing text and huds to the screen.
	// Lighting will be re-enabled next time through.
	glDisable(GL_LIGHTING);

	// Draw the console if open
	if( consoleActive )
		con->Draw();

	stringstream s;
	s << "FPS: " << frameRate;
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

// Handles keyboard input from normal text keys
void Scene::keyPressedEvent(unsigned char key, int x, int y)	{
//	cout << "Key pressed: " << key << endl;
	if( consoleActive )	{	// send key input to console
		switch(key)	{
			case CONSOLE_KEY:	// deactivate console
				consoleActive = !consoleActive;
				break;
			case ESC_KEY:
				con->clearInput();
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
			case ESC_KEY:
				cleanExit();
				break;
		}
	}
}



// handles keyboard input from special keys
void Scene::specialKeyPressedEvent(int key, int x, int y)	{
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

void Scene::mouseEvent(int button, int state, int x, int y)	{
	if( isPicking )	{
		if( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )	{
			cout << "Entering pick mode." << endl;
			startPicking(x, y);
			renderBSPTree(bspRoot);
			int hits = stopPicking();

			if( hits )	{
				processHits(hits, selectBuf);
			}
			else
				cout << "No hits occurred." << endl;
		}
	}
}


// BEGIN PICKING SHIT

void Scene::startPicking(int cursorX, int cursorY) {
	GLint viewport[4];

	glSelectBuffer(BUFSIZE,selectBuf);
	glRenderMode(GL_SELECT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glGetIntegerv(GL_VIEWPORT,viewport);
	gluPickMatrix(cursorX,viewport[3]-cursorY,
			5,5,viewport);

	float ratio = 1.0 * sceneWidth / sceneHeight;

	gluPerspective(45,ratio,0.1,1000);
	glMatrixMode(GL_MODELVIEW);
	glInitNames();
}


int Scene::stopPicking() {
	int hits;

	// restoring the original projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glFlush();

	// returning to normal rendering mode
	hits = glRenderMode(GL_RENDER);

	return hits;
}

void Scene::processHits(int hits, GLuint selectBuf[])	{
	cout << "Selection hits: " << hits << endl;

	if( hits )	{
		cout << "Number of names for this polygon: " << selectBuf[0] << endl;

		if( selectBuf[0] >= 1 )	{
			cout << "Polygon Name: " << selectBuf[3] << endl;

			polygon_t* p = polygonByName[selectBuf[3]];

			if( p )	{
				if( p->selected )
					p->selected = false;
				else	{
					p->selected = true;
					for(int x=0; x < p->numPoints; x++)	{
						cout << "Texture Point [" << x << "]: ";
						VectorPrint2f(p->texpts[x]);
						cout << endl;
					}
				}
			}
			else	{
				cout << "Error: couldn't find polygon's reference." << endl;
			}

		}
		else
			cout << "Unnamed polygon." << endl;
	}
}

// END PICKING SHIT



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







