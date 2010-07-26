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

	unsigned int polygonCount;

	// TODO make this list a complete list of the polygons
	// in the bsp tree, right now its static as the original obj
	list<polygon_t*> *polygonList;
	MaterialManager* matsManager;
	bsp_node_t* bspRoot;

	// TEMP SELECTION STUFFS
	bool isPicking;
	GLuint selectBuf[BUFSIZE];
	map<GLuint, polygon_t*>	polygonByName;
	// END TEMP SELECTION STUFFS

	Scene(int width, int height);
	~Scene(void);

	void createBSP(string mapName);
	void LoadMap(string map);
	void generateBSP(bsp_node_t* root);

	// The bureaucratic work
	void addPolygon(polygon_t* p);
	void renderBSPTree(bsp_node_t* tree);
	void renderPolygonList(list<polygon_t*> polygons, bool selectionMode);

	void advance(clock_t milliseconds);		// TODO This needs to be pushed out to the game class

	// GL dominated routines
	void render(void);
	void resizeSceneSize(int width, int height);
	void performLighting();
	void drawPolygon(polygon_t* poly, bool selectMode);
	void glCachePolygon(polygon_t* polygon);

	// Picking things
	void doPick(int button, int state, int x, int y);
	void startPicking(int cursorX, int cursorY);
	int stopPicking();
	void processHits(int hits, GLuint selectBuf[]);
	void nameAndCachePolygons(bsp_node_t* bspNode);
	void buildPolygonMapByName(bsp_node_t* bspRootNode);

	void exit();


	// TO BE REMOVED
	void doItAgain();
};



#endif /* SCENE_H_ */
