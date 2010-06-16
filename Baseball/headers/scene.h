/*
 * scene.h
 *
 *  Created on: Feb 25, 2010
 *      Author: brof
 */

#include <string>

#ifndef SCENE_H_
#define SCENE_H_


// TODO MD2Model Example Code
//#include "md2model.h"


class Scene	{

// TODO MD2Model Example Code
//	MD2Model *m;

public:

	Scene(void);
	~Scene(void);
	void render(void);
	void advance(clock_t milliseconds);
	void doItAgain();
};



#endif /* SCENE_H_ */
