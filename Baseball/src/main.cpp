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
#include <sys/time.h>

#define MOUSELOOK	// For development, disable in release



#ifdef linux
#include <pthread.h>
#include <GL/glut.h>
#endif // linux

// Definitions
#define SCREEN_WIDTH 		800
#define SCREEN_HEIGHT 		600

float sceneAdvRate = 0.5;
float maxPossible_dt = 0.008f;
int pitchSpeed = 120;

#ifdef linux
pthread_t gameThread;
#endif // linux

float clearColor[] = {0.0, 0.12, 0.24, 0.0};


// important time keeping variable
long timeStamp;


Scene* curScene;
MaterialManager* materials;
ModelManager* models;
Game* game = NULL;

void start_game_thread(void* args);

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

// We don't use threading yet, need a resource locking model in place first.
/*
#ifdef linux	// TODO This same define should work for MacOS too.
	if( (errcode=pthread_create(&gameThread, NULL, start_game_thread, &args)) )
		cout << "Error: Couldn't create pthread, error code: " << errcode << endl;
#endif // linux
*/

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
	game->killGame();	// stops game thread

	delete game;
	delete materials;
	delete curScene;
	exit(0);
}

// END GLOABLS!


void start_game_thread(void* args)	{
	cout << "Starting game thread..." << endl;
	game->run();
	cout << "Game thread finished." << endl;
}


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
long curtime;
long Sys_Milliseconds (void)
{
	struct timeval tp;

	gettimeofday(&tp, NULL);

	if (!sys_timeBase)
	{
		sys_timeBase = tp.tv_sec;
		return tp.tv_usec*0.001f;
	}

	curtime = (tp.tv_sec - sys_timeBase)*1000 + (tp.tv_usec*0.001f);

	return curtime;
}


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
#ifdef MOUSELOOK
	glutSetCursor(GLUT_CURSOR_NONE);
#endif
}

void changeSize(int w, int h)	{
	curScene->resizeSceneSize(w,h);
}


void draw(void)
{
	static int frameCount = 0;
	static long lastFrameTime = Sys_Milliseconds();
	static long frameStamp = lastFrameTime;

	long curFrameTime = Sys_Milliseconds();
	timeStamp = curFrameTime;

	long timeSinceLastFrame = curFrameTime - lastFrameTime;

	// this gives 1 extra fps! (old: dt = ms / 1000.0f)
	// Lesson: AVOID DIVISION WHENEVER POSSIBLE!)
	float dSec = timeSinceLastFrame * 0.001f;

	sceneAdvRate = dSec;


#ifdef MOUSELOOK
	// center mouse
	glutWarpPointer(400, 300);
#endif

	// Draw the scene.
	curScene->render(dSec);

	if( game )
		game->advance(dSec);


	// START: Frame rate processing
	frameCount++;
	lastFrameTime = curFrameTime;

	if( (frameStamp+1000) <= curFrameTime )	{
		curScene->frameRate = frameCount;
		frameCount = 0;
		frameStamp = curFrameTime;
	}
	// END: frame rate processing

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

#ifdef MOUSELOOK
void passiveMouseMove(int x, int y)	{
	game->passiveMouseEvent(x,y);
}
#endif

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

#ifdef MOUSELOOK
	glutPassiveMotionFunc(passiveMouseMove);
#endif

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

	glutMainLoop();

	return 0;
}




