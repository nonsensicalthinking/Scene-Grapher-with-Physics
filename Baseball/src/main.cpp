/*
 * main.c
 *
 * This file should strictly contain OpenGL and GLUT functions any
 * simulations or movement are done in other files.
 *
 * Primary OpenGL drawing file.
 *
 *
 *  Created on: Feb 24, 2010
 *      Author: Derek Brooks
 *
 *
 *	ATTENTION ANYONE CREATING A GAME WITH THIS RENDERING ENGINE
 *	ATTENTION There is only a couple small modifications you need to do
 *	ATTENTION to this file to get what you want.
 *	ATTENTION The function is called LoadGame and everything between the
 *	ATTENTION clearly defined editing area around that is free game.
 *	ATTENTION You must derive a new class from the Game class and overload
 *	ATTENTION its functions to make it all work but there you go.
 *
 *
 */

// The over all TODO list for the program
//
// TODO Devise strategy for sending network data, delta packets maybe?
//		Select model for hosting games, design database for statistics.
//
// TODO Implement programmable shaders
//
// End the TODO List...for now.


#include "Scene.h"
#include "MaterialManager.h"
#include "keys.h"			// Key presses defined
#include "GameTest.h"		// INCLUDE GAME HEADER HERE
#include <stdlib.h>
#include <ctime>
#include <iostream>

#define __LINUX__

#ifdef __LINUX__
#include <pthread.h>
#include <sys/time.h>
#include <GL/glut.h>
#endif // __LINUX__

// Definitions
#define SCREEN_WIDTH 		800
#define SCREEN_HEIGHT 		600

int sceneAdvRate = 3;
int pitchSpeed = 120;

#ifdef __LINUX__
pthread_t gameThread;
#endif // __LINUX__

float clearColor[] = {0.0, 0.12, 0.24, 0.0};

// Global variables
time_t lastFrameTime;
int frameRate;

Scene* curScene;
MaterialManager* materials;
Game* game = NULL;	// C++ gives us inheritance, hooray!

void* start_game_thread(void* args);	// Func defined below this



///////////////////////////////////////////////////////////////////////////////////////
// IF YOU ARE MODDING THIS ENGINE YOU DON'T NEED TO MODIFY THIS FILE ABOVE THIS LINE //
///////////////////////////////////////////////////////////////////////////////////////

#define GAME_TITLE	"Baseball something or other, no name given yet."

void LoadGame()	{
	// GAME CODE GOES HERE!

	// load game
	game = new SpecialGame();

	int args;
	int errcode;

///////////////////////////////////////////////////////////////////////////////////////
// IF YOU ARE MODDING THIS ENGINE YOU DON'T NEED TO MODIFY THIS FILE BELOW THIS LINE //
///////////////////////////////////////////////////////////////////////////////////////
// Trying something here...
/*
#ifdef __LINUX__
	if( (errcode=pthread_create(&gameThread, NULL, start_game_thread, &args)) )
		cout << "Error: Couldn't create pthread, error code: " << errcode << endl;
#endif // __LINUX__
*/
}



void* start_game_thread(void* args)	{
	cout << "Starting game thread..." << endl;
	game->run();
	cout << "Game thread finished." << endl;
}

MaterialManager* getMaterialManager()	{
	return materials;
}

Scene* getScene()	{
	return curScene;
}

Game* getGame()	{
	return game;
}


void cleanExit()	{
	game->killGame();	// stop game thread first

	delete game;
	delete materials;
	delete curScene;
	exit(0);
}

// END GLOABLS!

#ifdef __LINUX__
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
#endif // __LINUX__

void init()	{

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glEnable(GL_CULL_FACE);

	materials = new MaterialManager();
	curScene = new Scene(SCREEN_WIDTH, SCREEN_HEIGHT);

	LoadGame();	// End user game code loaded here
				// see the function at the top of file.
}

void changeSize(int w, int h)	{
	curScene->resizeSceneSize(w,h);
}

void draw(void)
{
	static int frameCount = 0;

	int curFrameTime = Sys_Milliseconds();

	long timeSinceLastFrame = curFrameTime - lastFrameTime;

	// Draw the scene.
	curScene->render();

	if( game != NULL )
		game->advance(sceneAdvRate);

	// Tabulate frame rate
	frameCount++;
	if( (lastFrameTime+1000) <= curFrameTime )	{
		curScene->frameRate = frameCount;
		frameCount = 0;
		lastFrameTime = curFrameTime;
	}
}

void processMouse(int button, int state, int x, int y)	{
	game->mouseEvent(button, state, x, y);
}

void processNormalKeys(unsigned char key, int x, int y)	{
	game->keyPressedEvent(key, x, y);
}

void processSpecialKeys(int key, int x, int y) {
	game->specialKeyPressedEvent(key, x, y);
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);

	glutCreateWindow(GAME_TITLE);

	init();

	// Set GLUT Call backs
	glutDisplayFunc(draw);
	glutIdleFunc(draw);
	glutReshapeFunc(changeSize);
	glutMouseFunc(processMouse);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);

	// Process startup commands.
	int processed = 1;	// start at the first real argument
	// Odd indices are commands, even indices are values of said commands
	while( argc > processed )	{
		if( !strcmp(argv[processed], "+map") )	{
			game->load(argv[processed+1]);
			cout << "Starting map: " << argv[processed+1] << endl;
		}
		else if( !strcmp(argv[processed], "+echo") )	{
			cout << "Echoing: " << argv[processed+1] << endl;
		}
		else	{
			cout << "Invalid startup argument: " << argv[processed] << endl;
		}

		processed += 2;
	}


	// This rodeo is fixin' to kick off...
	glutMainLoop();

	return 0;
}




