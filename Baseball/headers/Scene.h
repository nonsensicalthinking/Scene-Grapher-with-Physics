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

	// TODO Incorporate the texture manager and console permanently in this class
	MaterialManager* matsManager;
	ModelManager* modelManager;

	// TODO Remove this, make the sky part of the map
	GLUquadric* sky;
	GLuint skyCacheID;

	void fullScreen(bool full);
	void resizeSceneSize(int width, int height);
	void performLighting();
	void drawPolygon(polygon_t* poly);
	void cacheSky();
	void glCachePolygon(polygon_t* polygon);
	void nameAndCachePolygons(bsp_node_t* bspNode);
	void unCachePolygons(bsp_node_t* bspNode);
	void drawEntity(float dt, entity_t* ent);
	void drawEntityList(float dt, list<entity_t*> mlist);
	void renderBSPTree(bsp_node_t* tree);
	void renderPolygonList(list<polygon_t*> polygons);
	void render(float dt);

	void reset();
	void exit();

	Scene(int width, int height);
	~Scene(void);
};



#endif /* SCENE_H_ */
