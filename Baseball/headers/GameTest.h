/*
 * GameTest.h
 *
 *  Created on: Jul 26, 2010
 *      Author: brof
 */
#include "Game.h"
#include "Scene.h"

#define CAM_MOVE_RATE 20

extern Scene* getScene();


// inherit the game class
class SpecialGame : public Game	{


public:

	SpecialGame() : Game()	{

	}

	~SpecialGame()	{

	}

	virtual void run()	{
		cout << "Called special run function" << endl;
	}

	virtual void advance(long ms)	{
/*		// Time work, used for Simulation work
		// dt Is The Time Interval (As Seconds) From The Previous Frame To The Current Frame.
		// dt Will Be Used To Iterate Simulation Values Such As Velocity And Position Of Masses.
		float dt = ms / 1000.0f;							// Let's Convert Milliseconds To Seconds
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
		*/
	}

	// Event handlers
	// Handles keyboard input from normal text keys
	virtual void keyPressedEvent(unsigned char key, int x, int y)	{
		Scene* curScene = getScene();

		if( curScene->con->consoleActive )	{	// send key input to console
			switch(key)	{
				case CONSOLE_KEY:	// deactivate console
					curScene->con->consoleActive = !curScene->con->consoleActive;
					break;
				case ESC_KEY:
					curScene->con->clearInput();
					break;
				default:	// add to input line
					curScene->con->appendToInput(key);
			}
		}
		else	{	// Don't send key input to console
			switch(key)	{
				case '`':	// active console
					curScene->con->consoleActive = !curScene->con->consoleActive;
					break;
				case 'a':	// omg our first control over the scene!
					curScene->cam->moveCameraLeft(CAM_MOVE_RATE);
					break;
				case 'd':	// omg our first control over the scene!
					curScene->cam->moveCameraRight(CAM_MOVE_RATE);
					break;
				case 'w':	// omg our first control over the scene!
					curScene->cam->moveCameraForward(CAM_MOVE_RATE);
					break;
				case 's':	// omg our first control over the scene!
					curScene->cam->moveCameraBack(CAM_MOVE_RATE);
					break;
				case 'q':	// omg our first control over the scene!
					curScene->cam->moveCameraUp(CAM_MOVE_RATE);
					break;
				case 'e':	// omg our first control over the scene!
					curScene->cam->moveCameraDown(CAM_MOVE_RATE);
					break;
				case 'z':
					curScene->doItAgain();
					break;
				case ESC_KEY:
					curScene->exit();
					break;
			}
		}
	}



	// handles keyboard input from special keys
	virtual void specialKeyPressedEvent(int key, int x, int y)	{
	//	cout << "Special Key Pressed: " << key << endl;
		Scene* curScene = getScene();

		if( curScene->con->consoleActive )	{	// send key input to console
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
					curScene->con->scrollUp();
					break;
				case PAGE_DOWN_KEY:
					curScene->con->scrollDown();
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

	virtual void mouseEvent(int button, int state, int x, int y)	{
		Scene* curScene = getScene();
		curScene->doPick(button, state, x, y);
	}


};
