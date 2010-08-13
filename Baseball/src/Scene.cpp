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
 * Fix lighting crap
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

#include "GameTest.h"

#include <sstream>
#include <GL/glut.h>


using namespace std;

#define Z_NEAR		0.1
#define Z_FAR		700	// this happens to be the diameter of the "skybox" for now



// TODO have this draw a model instead of just a point
void Scene::drawEntity(entity_t* ent)	{
	if( ent->hasExpired )
		return;

	glPushMatrix();
    glPointSize(5);
    glBegin(GL_POINTS);
            glVertex3f(ent->mass->pos[0], ent->mass->pos[1], ent->mass->pos[2]);
    glEnd();
    glPopMatrix();
}

void Scene::drawEntityList(list<entity_t*> mlist)	{
	list<entity_t*>::iterator itr;
	for(itr=mlist.begin(); itr != mlist.end(); itr++)
		drawEntity((*itr));
}

void Scene::setEntityList(list<entity_t*> mlist)	{
	entList = mlist;
}

void Scene::submitBSPTree(bsp_node_t* root)	{
	bspRoot = root;
}

void Scene::cacheSky()	{
    skyCacheID = glGenLists(1);
    glNewList(skyCacheID, GL_COMPILE);
    glPushMatrix();
	matsManager->enableSphereMapping();
	matsManager->bindTexture(SKY_TEXTURE);
	gluSphere(sky, 350, 10, 5);
	matsManager->disableSphereMapping();
	glPopMatrix();
    glEndList();
}

Scene::Scene(int width, int height)
{
	sceneWidth = width;
	sceneHeight = height;
	con = new Console(width,height);
	con->consoleActive = false;
	matsManager = getMaterialManager();
	cam = new Camera();
	polygonCount = 0;	// count of static polygons in the entire scene
	bspRoot = NULL;
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
	gluPerspective(cam->fov, ratio, Z_NEAR, Z_FAR);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(	cam->origin[0], cam->origin[1], cam->origin[2],	// camera origin
				cam->dir[0], cam->dir[1], cam->dir[2],		// eye looking @ this vertex
				cam->up[0], cam->up[1], cam->up[2]);	// up direction
}


void Scene::performLighting()	{
	GLfloat df = 1.0;
	GLfloat amb[]=	{0.5, 0.5, 0.5, 1};   		//global ambient

	GLfloat amb2[]=	{1, 1, 1, 1};  		//ambiance of light source
	GLfloat diff[]=	{1.0, 1.0, 1.0, 1.0};	// diffuse light
	GLfloat spec[]=	{1.0, 1.0, 1.0, 1.0};      	//sets specular highlight
	GLfloat posl[]=	{-21, 33, 18, 1};            //position of light source

//	GLfloat posL1[] = {0, 5, 0};
//	GLfloat spotDir[] = {0, -1, 0};



	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
//	glMaterialfv(GL_FRONT, GL_SHININESS, &df);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// global ambiance
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
	glShadeModel(GL_SMOOTH);

	// Light 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb2);
//	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
	glLightfv(GL_LIGHT0, GL_POSITION, posl);

//	glEnable(GL_COLOR_MATERIAL);
//	glColorMaterial(GL_FRONT, GL_AMBIENT);

	// Light 1
//	glLightfv(GL_LIGHT1, GL_AMBIENT, amb2);
//	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDir);
//	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 15.f);
//	glLightfv(GL_LIGHT1, GL_SPECULAR, spec);
//	glLightfv(GL_LIGHT1, GL_POSITION, posL1);
//	glEnable(GL_LIGHT1);


}

void Scene::glCachePolygon(polygon_t* polygon)	{
    polygon->glCacheID = glGenLists(1);
    glNewList(polygon->glCacheID, GL_COMPILE);
    drawPolygon(polygon);
    glEndList();
    polygon->glCached = true;
}


void Scene::drawPolygon(polygon_t* poly)	{
	if( poly->glCached )	{
		glCallList(poly->glCacheID);
	}
	else	{
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
}

void Scene::renderPolygonList(list<polygon_t*> polygons)
{
	for(list<polygon_t*>::iterator itr = polygons.begin(); itr != polygons.end(); itr++)
		drawPolygon((*itr));
}

void Scene::renderBSPTree(bsp_node_t* tree)	{

	if( !tree )
		return;

	if( tree->isLeaf() )	{
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

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// position camera
	gluLookAt(	cam->origin[0], cam->origin[1], cam->origin[2],	// camera origin
				cam->dir[0], cam->dir[1], cam->dir[2],		// eye looking @ this vertex
				cam->up[0], cam->up[1], cam->up[2]);	// up direction

	performLighting();

	//////////////////////////////////
	// 		Scene Drawing Area		//
	// v	v	v	v	v	v	v	//
	if( bspRoot )	{
		// TODO find a better way to draw background sky
		glPushMatrix();
		// Make the sky emit light!
		GLfloat emis[] = {0.4, 0.4, 0.4};
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emis);
		glCallList(skyCacheID);
		glPopMatrix();

		renderBSPTree(bspRoot);

		// as of right now we have to draw the entities
		// Separately because they're nearly always
		// being added and removed from the bsp tree to
		// check for collisions.
		drawEntityList(entList);
	}

	/////////////////////////////
	// Lighting Disabled Area  //
	// v	v	v	v	v	v  //
	// Disable lighting for drawing text and huds to the screen.
	// Lighting will be re-enabled next time through.
//	glDisable(GL_LIGHTING);

	// Draw the console if open
	if( con->consoleActive )
		con->Draw();


	// FIXME TEMPORARY, FIND A BETTER WAY TO DO THIS
	stringstream s;
	s << "FPS: " << frameRate;
	con->font->glPrint(0, 0, s.str().c_str(), 0);
	// END FIXME


	glutSwapBuffers();	// swap out the display buffer with our new scene
}

void Scene::nameAndCachePolygons(bsp_node_t* bspNode)	{
	list<polygon_t*>::iterator itr;

	if( bspNode->isLeaf() )	{
		for(itr = bspNode->beginPolyListItr(); itr != bspNode->endPolyListItr(); itr++)	{
			glCachePolygon(*itr);
			(*itr)->polyID = (*itr)->glCacheID;
			polygonCount++;	// just for stats, may be removed later
		}
	}
	else	{
		nameAndCachePolygons(bspNode->front);
		nameAndCachePolygons(bspNode->back);
	}
}

void Scene::exit()	{
	cleanExit();
}



/*
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
*/



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




