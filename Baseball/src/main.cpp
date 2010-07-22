/*
 * main.c
 *
 * This file should strictly contain opengl functions any
 * simulations or movement are done in other files.
 *
 * Primary OpenGL drawing file.
 *
 *
 *  Created on: Feb 24, 2010
 *      Author: Derek Brooks
 */

// The over all TODO list for the program
//
// PRIORITY *THIS WILL FIX TEXTURING OF BSP OBJECTS*
// TODO When splitting polygons in the bsp routines
//		the texture coordinates are not copied over
//		also they must be re-calculated base on the
//		new polygon.
//
// TODO Fix Camera Class so it can do all directions
//		with focal point adjustment too.
//
// TODO Implement collision detection
//
// TODO Devise strategy for sending network data, delta packets maybe?
//		Select model for hosting games, design database for statistics.
//
// TODO Implement programmable shaders
//
// End the TODO List...for now.

/*
 * Ideas on how this might all fit together.
 *
 * Obviously you need some sort of map builder, when the map is built it is textured
 * then when the map is complete then "compile" the BSP, this will involve splitting
 * the larger polygons into smaller ones that fit in the division spaces, when we cut
 * up the polygons we should be able to get its relative position on the over all
 * texture, we should save this information for loading and texturing the split polygon
 *
 * From what I've seen, be it as it may drunk, things are kept modular in the q1 engine
 * meaning generic functions for loading textures, binding textures, sampling textures
 * etc...etc...
 *
 * From here we can begin drawing a scene and testing for collisions, meaning we can
 * start working on collision detection and refraction for baseballs etc
 *
 * I think the best idea is obviously to keep as much of the rendering, loading, and
 * texturing seperate from the baseball game itself, meaning physics are part of baseball
 * model drawing is part of the renderer, etc...try your best to separate these.
 *
 * Once things are loaded and collisions work we can start playing with variables
 * which is where requiring the console will come into action
 *
 *
 *
 */


#include "Scene.h"
#include "MaterialManager.h"
#include "keys.h"			// Key presses defined
#include <stdlib.h>
#include <ctime>
#include <iostream>
#include <sys/time.h>
#include <GL/glut.h>

// Definitions
#define SCREEN_WIDTH 		800
#define SCREEN_HEIGHT 		600

// The rate at which to advance the scene
// FIXME: This might have implications in other
// operations such as animation.
#define SCENE_ADVANCE_RATE	5

float clearColor[] = {0.0, 0.12, 0.24, 0.0};

// Global variables
time_t lastFrameTime;
int frameRate;

Scene* curScene;
MaterialManager* textures;

MaterialManager* getTextureManager()	{
	return textures;
}

Scene* getScene()	{
	return curScene;
}

void cleanExit()	{
	delete textures;
	delete curScene;
	exit(0);
}

// END GLOABLS!


/* From Quake 3 Arena
================
Sys_Milliseconds
================
*/
/* base time in seconds, that's our origin
   timeval:tv_sec is an int:
   assuming this wraps every 0x7fffffff - ~68 years since the Epoch (1970) - we're safe till 2038
   using unsigned long data type to work right with Sys_XTimeToSysTime */
unsigned long sys_timeBase = 0;
/* current time in ms, using sys_timeBase as origin
   NOTE: sys_timeBase*1000 + curtime -> ms since the Epoch
     0x7fffffff ms - ~24 days
   although timeval:tv_usec is an int, I'm not sure wether it is actually used as an unsigned int
     (which would affect the wrap period) */
int curtime;
int Sys_Milliseconds (void)
{
	struct timeval tp;

	gettimeofday(&tp, NULL);

	if (!sys_timeBase)
	{
		sys_timeBase = tp.tv_sec;
		return tp.tv_usec/1000;
	}

	curtime = (tp.tv_sec - sys_timeBase)*1000 + tp.tv_usec/1000;

	return curtime;
}



void init()	{
	glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);

	textures = new MaterialManager();
	curScene = new Scene(SCREEN_WIDTH, SCREEN_HEIGHT);
	curScene->createBSP();
}



void changeSize(int w, int h)	{
	curScene->resizeSceneSize(w,h);
}


void draw(void)
{
	static int frameCount = 0;

	// Based on rate of change, advance the physical objects
	// in the scene then draw 'em out.
	curScene->advance(SCENE_ADVANCE_RATE);
	curScene->render();

	// Tabulate frame rate
	int curFrameTime = Sys_Milliseconds();
	frameCount++;
	if( (lastFrameTime+1000) <= curFrameTime )	{
		curScene->frameRate = frameCount;
		frameCount = 0;
		lastFrameTime = curFrameTime;
	}
}

void processMouse(int button, int state, int x, int y)	{
	curScene->mouseEvent(button, state, x, y);
}

void processNormalKeys(unsigned char key, int x, int y)
{
	curScene->keyPressedEvent(key, x, y);
}

void processSpecialKeys(int key, int x, int y) {
	curScene->specialKeyPressedEvent(key, x, y);
}



int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow("Not Yet Titled, baseball something or other.");
	glutDisplayFunc(draw);
	glutIdleFunc(draw);
	glutReshapeFunc(changeSize);

	//adding here the setting of keyboard processing
	glutMouseFunc(processMouse);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	init();
	glutMainLoop();

	return 0;
}






