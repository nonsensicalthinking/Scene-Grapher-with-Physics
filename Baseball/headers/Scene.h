/*
 * scene.h
 *
 *  Created on: Feb 25, 2010
 *      Author: Derek Brooks
 */

#include <string>
#include <list>
#include <map>
#include <vector>

#include "shared.h"
#include "MaterialManager.h"
#include "Console.h"
#include "Camera.h"
#include "bsptree.h"
#include "ent.h"
#include "ModelManager.h"

extern void cleanExit();	// defined in main.cpp

#ifndef SCENE_H_
#define SCENE_H_


class Scene	{
public:
	int frameRate;

	int sceneWidth;
	int sceneHeight;


	Console* con;

	Camera* cam;
	vector<Camera*> cameras;

	unsigned int polygonCount;

	MaterialManager* matsManager;
	ModelManager* modelManager;


	// TODO Remove this, make the sky part of the map
	GLUquadric* sky;
	GLuint skyCacheID;

	Scene(int width, int height);
	~Scene(void);

	void drawEntity(float dt, entity_t* ent);
	void drawEntityList(float dt, list<entity_t*> mlist);

	void renderBSPTree(bsp_node_t* tree);
	void renderPolygonList(list<polygon_t*> polygons);

	void fullScreen(bool full);

	// GL dominated routines
	void render(float dt);
	void resizeSceneSize(int width, int height);
	void performLighting();
	void drawPolygon(polygon_t* poly);
	void nameAndCachePolygons(bsp_node_t* bspNode);
	void unCachePolygons(bsp_node_t* bspNode);
	void glCachePolygon(polygon_t* polygon);
	void cacheSky();

	void exit();
	void reset();
};



#endif /* SCENE_H_ */
