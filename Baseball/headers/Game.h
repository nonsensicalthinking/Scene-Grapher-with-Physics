/*
 * Game.cpp
 *
 *  Created on: Jul 26, 2010
 *      Author: brof
 */

#include "Scene.h"
#include <iostream>

#define CAM_MOVE_RATE 20

#define SCENE_ADVANCE_RATE	5

using namespace std;

extern Scene* getScene();


class Game	{

private:
	bool gameOn;

public:

	Game()	{

	}

	~Game()	{

	}

	void killGame()	{
		gameOn = false;
	}

	// Run will call load, this is where the maps are loaded from
	void run()	{
		load();
		gameOn = true;

		while( gameOn )	{
			advance(SCENE_ADVANCE_RATE);
		}

		cout << "Game thread ended." << endl;
	}


	// Event handlers
	void keyPressedEvent(unsigned char key, int x, int y)	{
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
			keyPressed(key, x, y);
		}
	}

	void specialKeyPressedEvent(int key, int x, int y)	{
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
		else	{
			specialKeyPressed(key, x, y);
		}
	}


	// Function stubs from here down.

	// This is to be overridden
	virtual void load()	{

	}

	// This is to be overridden
	virtual void advance(long ms)	{

	}

	// This is to be overridden
	virtual void keyPressed(unsigned char key, int x, int y)	{

	}

	// This is to be overridden
	virtual void specialKeyPressed(int key, int x, int y)	{

	}

	// This is to be overridden
	virtual void mouseEvent(int button, int state, int x, int y)	{

	}


};
