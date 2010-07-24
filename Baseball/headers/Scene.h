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
	bool consoleActive;

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
	void glCachePolygon(polygon_t* polygon);
	void drawPolygon(polygon_t* poly, bool selectMode);
	void renderPolygonList(list<polygon_t*> polygons, bool selectionMode);
	void advance(clock_t milliseconds);
	long Syscmd(string s);
	void addPolygon(polygon_t* p);
	void LoadMap(string map);
	void performLighting();
	void exit();
	void generateBSP(bsp_node_t* root);
	void renderBSPTree(bsp_node_t* tree);
	void createBSP();
	void resizeSceneSize(int width, int height);
	void startPicking(int cursorX, int cursorY);
	int stopPicking();
	void processHits(int hits, GLuint selectBuf[]);
	void namePolygons(bsp_node_t* bspNode);
	void buildPolygonByNameMap(bsp_node_t* bspRootNode);

	// Event handlers
	void keyPressedEvent(unsigned char key, int x, int y);
	void specialKeyPressedEvent(int key, int x, int y);
	void mouseEvent(int button, int state, int x, int y);


	// TO BE REMOVED
	void doItAgain();
};



#endif /* SCENE_H_ */
