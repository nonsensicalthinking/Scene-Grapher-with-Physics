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
 * TODO fix lighting to allow diffuse light
 * TODO Tend to other todo's further down in file
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

// TODO Put this some place else!!
#define RADIUS_OF_BASEBALL 0.0762  // 0.448056	// in meters obviously

// CVARS
int r_drawWireFrame = 0;

// Global getters
extern Game* getGame();

void Scene::fullScreen(bool full)	{
	// TODO Get full screen working
}

void Scene::resizeSceneSize(int width, int height)	{
	// Prevent a divide by zero, when window is too short
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

// This function is still a work in progress
void Scene::performLighting()	{
	GLfloat df = 1.0;
	GLfloat amb[]=	{0.5, 0.5, 0.5, 1};   		//global ambient

	GLfloat amb2[]=	{1, 1, 1, 1};  		//ambiance of light source
	GLfloat diff[]=	{1.0, 1.0, 1.0, 1.0};	// diffuse light
	GLfloat spec[]=	{1.0, 1.0, 1.0, 1.0};      	//sets specular highlight
	GLfloat posl[]=	{-21, 75, 18, 1};            //position of light source

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


void Scene::drawEntity(float dt, entity_t* ent)	{
	if( ent->hasExpired )
		return;

	if( ent->model )	{
		glPushMatrix();
		glTranslatef(ent->mass->pos[0], ent->mass->pos[1], ent->mass->pos[2]);
		glRotatef(ent->facing[0], 1.0, 0, 0);
		glRotatef(ent->facing[1], 0, 1.0, 0);
		glRotatef(ent->facing[2], 0, 0, 1.0);
		ent->model->advance(dt);
		ent->model->draw();
		glPopMatrix();
	}
	else	{
		glPushMatrix();
			glTranslatef(ent->mass->pos[0], ent->mass->pos[1], ent->mass->pos[2]);
			glutSolidSphere(RADIUS_OF_BASEBALL, 10, 5);
	    glPopMatrix();
	}
}

void Scene::drawEntityList(float dt, list<entity_t*> mlist)	{
	list<entity_t*>::iterator itr;
	for(itr=mlist.begin(); itr != mlist.end(); itr++)
		drawEntity(dt,(*itr));
}

void Scene::drawPolygon(polygon_t* poly)	{
	if( poly->glCached )	{
		glCallList(poly->glCacheID);
	}
	else	{
		glPushMatrix();
		if( r_drawWireFrame )	{	// draw wire frame for polygons
			glBegin(GL_LINE_STRIP);
			for(int x=0; x < poly->numPoints; x++)	{
				glVertex3f(poly->points[x][0], poly->points[x][1], poly->points[x][2]);
			}
			glEnd();
		}
		else	{
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
		}

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

void Scene::render(float dt)
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
	bsp_node_t* bspTree = getGame()->getBSPTree();

	if( bspTree )	{
		// TODO find a better way to draw background sky
		glPushMatrix();
		// Make the sky emit light!
		GLfloat emis[] = {0.4, 0.4, 0.4};
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emis);
		glCallList(skyCacheID);
		glPopMatrix();

		renderBSPTree(bspTree);

		// as of right now we have to draw the entities
		// Separately because they're nearly always
		// being added and removed from the bsp tree to
		// check for collisions.
		drawEntityList(dt, getGame()->getEntList());
	}



	// FIXME: this is off to make the console bg black instead of
	// blinding white.

	// Lighting Disabled Area:
	// Disable lighting for drawing text and huds to the screen.
	// Lighting will be re-enabled next time through.
	glDisable(GL_LIGHTING);

	// Draw the console if open
	if( con->consoleActive )
		con->Draw();


	stringstream s;
	s << "FPS: " << frameRate;
	con->font->glPrint(0, 0, s.str().c_str(), 0);


	glutSwapBuffers();	// swap out the display buffer with our new scene
}

// TODO Make this part of the map some how instead of
// separately like this.
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

void Scene::glCachePolygon(polygon_t* polygon)	{
    polygon->glCacheID = glGenLists(1);
    glNewList(polygon->glCacheID, GL_COMPILE);
    drawPolygon(polygon);
    glEndList();
    polygon->glCached = true;
}

void Scene::nameAndCachePolygons(bsp_node_t* bspNode)	{
	list<polygon_t*>::iterator itr;

	if( bspNode->isLeaf() )	{
		for(itr = bspNode->beginPolyListItr(); itr != bspNode->endPolyListItr(); itr++)	{
			glCachePolygon(*itr);
			(*itr)->polyID = (*itr)->glCacheID;
			polygonCount++;	// just for stats, may be removed later

			// This is a mem mgmt debug print
//			cout << "PolyCacheID: " << (*itr)->glCacheID << endl;
		}
	}
	else	{
		nameAndCachePolygons(bspNode->front);
		nameAndCachePolygons(bspNode->back);
	}
}

void Scene::unCachePolygons(bsp_node_t* bspNode)	{
	list<polygon_t*>::iterator itr;

	if( !bspNode )	{
#ifdef BSPDEBUG
		cout << "ERROR: unCachePolygons(): BSP NODE IS NULL DAMN YOU" << endl;
#endif
		return;
	}

	if( bspNode->isLeaf() )	{
#ifdef BSPDEBUG
		cout << "unCachePolygons(): Found a leaf uncaching " << bspNode->getPolygonList().size() << " polygons..." << endl;
#endif
		for(itr = bspNode->beginPolyListItr(); itr != bspNode->endPolyListItr(); itr++)	{
			int i = (*itr)->glCacheID;
			glDeleteLists(i, 1);
#ifdef BSPDEBUG
			cout << "unCachePolygons(): glCacheID: " << (*itr)->glCacheID << " cleared." << endl;
#endif
		}
	}
	else	{
#ifdef BSPDEBUG
		cout << "unCachePolygons(): Found node going front & back... following both" << endl;
#endif
		nameAndCachePolygons(bspNode->front);
		nameAndCachePolygons(bspNode->back);
	}
}

void Scene::reset()	{
	// unload textures, models, bsp tree
	bsp_node_t* bspTree = getGame()->getBSPTree();
	cout << "------------ UNLOADING RESOURCES -------------" << endl;
	cout << "-> unCaching Polygons...";
	unCachePolygons(bspTree);
	cout << "done." << endl;
	cout << "-> Freeing BSP Tree and its referenced resources...";
	deleteTree(bspTree);
	polygonCount = 0;
	cout << "done. (0 polygons in the scene)" << endl;

	cout << "-> Purging Materials...";
	matsManager->purgeMaterials();
	cout << "done" << endl;

	cout << "------- UNLOADING RESOURCES COMPLETED  -------" << endl;

}

void Scene::exit()	{
	cleanExit();
}

Scene::Scene(int width, int height)
{
	sceneWidth = width;
	sceneHeight = height;
	con = new Console(width,height);
	con->consoleActive = false;
	matsManager = getMaterialManager();
	modelManager = new ModelManager();
	polygonCount = 0;	// count of static polygons in the entire scene

	cam = new Camera();
	vec3_t p = {-45, 3, 36};
	vec3_t l = {0.684131, -0.198672, -0.701779};
	cam->setView(p, l);
	cameras.push_back(cam);

	Camera* sideCam = new Camera();
	vec3_t pos = {0, 23, 56};
	vec3_t look = {-0.0738074, -0.295523, -0.95248};
	sideCam->setView(pos, look);
	cameras.push_back(sideCam);
}


Scene::~Scene()
{
	reset();
	delete con;
}


