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
#include <map>
#include <vector>

#ifndef SCENE_H_
#define SCENE_H_

// TODO MD2Model Example Code
//#include "md2model.h"


#define BUFSIZE	20

extern void cleanExit();

class Scene	{

// TODO MD2Model Example Code
//	MD2Model *m;

public:
	int frameRate;
	int sceneWidth;
	int sceneHeight;
	Console* con;

	Camera* cam;
	vector<Camera*> *cameras;

	int polygonCount;
	bool isPicking;
	list<polygon_t*> *polygonList;
	MaterialManager* matsManager;

	// TEMP SELECTION STUFFS
	GLuint selectBuf[BUFSIZE];
	map<GLuint, polygon_t*>	polygonByName;
	// END TEMP SELECTION STUFFS

	Scene(int width, int height);
	~Scene(void);

	void render(void);
	void renderPolygonList(list<polygon_t*> polygons, bool selectionMode);
	void drawPolygon(polygon_t* poly, bool selectMode);
	void glCachePolygon(polygon_t* polygon);

	// This needs to be pushed out to the game class
	void advance(clock_t milliseconds);

	void LoadMap(string map);
	void addPolygon(polygon_t* p);
	void performLighting();
	void exit();
	void generateBSP(bsp_node_t* root);
	void renderBSPTree(bsp_node_t* tree);
	void createBSP();
	void resizeSceneSize(int width, int height);

	// Picking things
	void doPick(int button, int state, int x, int y);
	void startPicking(int cursorX, int cursorY);
	int stopPicking();
	void processHits(int hits, GLuint selectBuf[]);
	void namePolygons(bsp_node_t* bspNode);
	void buildPolygonMapByName(bsp_node_t* bspRootNode);



	// TO BE REMOVED
	void doItAgain();
};



#endif /* SCENE_H_ */
