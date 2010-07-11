/*
 * scene.h
 *
 *  Created on: Feb 25, 2010
 *      Author: Derek Brooks
 */

#include "shared.h"
#include "MaterialManager.h"
#include "Console.h"
#include "Camera.h"
#include "bsptree.h"
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
	void generateBSP(bsp_node_t* root);
	void renderBSPTree(bsp_node_t* tree);
	void createBSP();
	void resizeSceneSize(int width, int height);
	// TO BE REMOVED
	void doItAgain();
};



#endif /* SCENE_H_ */
