/*
 * scene.h
 *
 *  Created on: Feb 25, 2010
 *      Author: brof
 */

#include "shared.h"
#include "MaterialManager.h"
#include "Console.h"
#include "Camera.h"
#include <string>
#include <list>
#include <vector>

#ifndef SCENE_H_
#define SCENE_H_

// TODO MD2Model Example Code
//#include "md2model.h"

extern void cleanExit();
extern int getFrameRate();

class Scene	{

// TODO MD2Model Example Code
//	MD2Model *m;

public:
	int sceneWidth;
	int sceneHeight;
	Console* con;
	bool consoleActive;

	Camera* cam;
	vector<Camera*> *cameras;

	list<polygon_t*> *polygonList;
	MaterialManager* matsManager;

	Scene(int width, int height);
	~Scene(void);
	void render(void);
	void drawPolygon(polygon_t* poly);
	void renderPolygonList(list<polygon_t*> polygons);
	void advance(clock_t milliseconds);
	void keyPressed(unsigned char key);
	void specialKeyPressed(int key, int x, int y);
	long Syscmd(string s);
	void addPolygon(polygon_t* p);
	void LoadMap(string map);
	void performLighting();
	void exit();

	// TO BE REMOVED
	void doItAgain();
};



#endif /* SCENE_H_ */
