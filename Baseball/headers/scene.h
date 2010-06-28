/*
 * scene.h
 *
 *  Created on: Feb 25, 2010
 *      Author: brof
 */

#include "Console.h"
#include <string>

#ifndef SCENE_H_
#define SCENE_H_

// TODO MD2Model Example Code
//#include "md2model.h"


class Scene	{

// TODO MD2Model Example Code
//	MD2Model *m;

public:
	Console* con;
	bool consoleActive;


	Scene(int width, int height);
	~Scene(void);
	void render(void);
	void advance(clock_t milliseconds);
	void doItAgain();
	void keyPressed(unsigned char key);
	void specialKeyPressed(int key, int x, int y);
};



#endif /* SCENE_H_ */
