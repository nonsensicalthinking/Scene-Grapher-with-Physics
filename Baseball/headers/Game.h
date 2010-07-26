/*
 * Game.cpp
 *
 *  Created on: Jul 26, 2010
 *      Author: brof
 */

#include <iostream>

using namespace std;



class Game	{

public:

	Game()	{

	}

	~Game()	{

	}

	virtual void run()	{

	}

	virtual void advance(long ms)	{

	}

	// Event handlers
	virtual void keyPressedEvent(unsigned char key, int x, int y)	{

	}

	virtual void specialKeyPressedEvent(int key, int x, int y)	{

	}
	virtual void mouseEvent(int button, int state, int x, int y)	{

	}



};
