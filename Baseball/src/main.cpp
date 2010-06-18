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
 *      Author: brof
 */

// BRIGHT IDEA
// With regards to the mapping bsp split objects with one whole texture
// the same principle is in effect for the texture mapped fonts

// The TODO list...ooooo
//
//
// TODO Make Camera class to store camera info and
//		functions used to move camera about the scene
//
// TODO Implement BSP Tree for collision detection and
// 		drawing visible objects??
//
// TODO Implement simple texturing system so we can texture
//		parts of a texture on a particular polygon
//
// TODO Make Vector3d one unified container, as of right now
//		there are 3 kinds of vector containers: vec3_t, vec3f, Vector3D
//		pick one and stick with it asshole...
//
// TODO Create some sort of glPrint function, or rip one off and offer
//		credit to the creator in exchange..,
//
// TODO Create console class so we can modify cvars and view errors printed
//		inside the simulation
//
//
// End the TODO List....thank god

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
 * From here we can bein drawing a scene and testing for collisions, meaning we can
 * start working on collision detection and refraction for baseballs etc
 *
 * I think the best idea is obviously to keep as much of the rendering, loading, and
 * texturing seperate from the baseball game itself, meaning physics are part of baseball
 * model drawing is part of the renderer, etc...try your best to seperate these.
 *
 * Once things are loaded and collisions work we can start playing with variables
 * which is where requiring the console will come into action
 *
 *
 *
 */


#include <stdlib.h>
#include <ctime>
#include <iostream>
#include "scene.h"
#include "keys.h"			// Key presses defined
#include <GL/glut.h>


// Definitions
#define SCREEN_WIDTH 		800
#define SCREEN_HEIGHT 		600

// The rate at which to advance the scene
// FIXME: This might have implications in other
// operations such as animation.
#define SCENE_ADVANCE_RATE	5


// Global variables
time_t lastFrameTime;
double frameRate;

// MAKE CLASS FOR CAMERA AND MOVEMENT
GLfloat camOrigin[] = {0.0, 10.0, 50.0};		// Base point of camera
GLfloat camDirection[] = {-5.0, 10.0, 0.0 };	// A point some distance directly infront of camera
GLfloat camOrientation[] = {0.0, 1.0, 0.0};	// Which way is up?? =P

Scene *curScene;

void init()	{
	curScene = new Scene();	// Create a new scene to feed me!
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	glClearColor(0,0,0,0);
  	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}


void lighting()	{
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




void changeSize(int w, int h)	{
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;

	float ratio = 1.0* w / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45,ratio,1,1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(	camOrigin[0], camOrigin[1], camOrigin[2],	// camera origin
				camDirection[0], camDirection[1], camDirection[2],		// eye looking @ this vertex
				camOrientation[0], camOrientation[1], camOrientation[2]);	// up direction

}



void draw(void)
{
	// Perform lighting on the scene
	lighting();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// position camera
	gluLookAt(	camOrigin[0], camOrigin[1], camOrigin[2],	// camera origin
				camDirection[0], camDirection[1], camDirection[2],		// eye looking @ this vertex
				camOrientation[0], camOrientation[1], camOrientation[2]);	// up direction

	// Advance scene's physical positioning and draw it
	curScene->advance(SCENE_ADVANCE_RATE);
	curScene->render();
}



void processNormalKeys(unsigned char key, int x, int y)
{
	if (key == ESC_KEY)
		exit(0);

	if( key == A_KEY )	{
		curScene->doItAgain();
	}
	else if( key == D_KEY )	{

	}
	else if( key == S_KEY )	{

	}
	else if( key == W_KEY )	{

	}
}

void processSpecialKeys(int key, int x, int y) {

	switch(key) {
//		case GLUT_KEY_F1 : red = 1.0; green = 0.0; blue = 0.0; break;
//		case GLUT_KEY_F2 : red = 0.0; green = 1.0; blue = 0.0; break;
//		case GLUT_KEY_F3 : red = 0.0; green = 0.0; blue = 1.0; break;
	}
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
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	init();
	glutMainLoop();

	return 0;
}
