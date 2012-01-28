/*
 * BSPTree.h
 *
 *  Created on: Jun 4, 2010
 *      Author: Derek Brooks
 */

#include "ent.h"


#ifndef BSPTREE_H_
#define BSPTREE_H_
#include <list>

#define BSP_RECURSION_DEPTH		11

#define		PLANE_NORMAL_X		0
#define 	PLANE_NORMAL_Y		1
#define		PLANE_NORMAL_Z		2

//#define BSPDEBUG



typedef struct bsp_node_s	{
#ifdef BSPDEBUG
	int nodeNumber;
#endif
	bool root;
	plane_t* partition;
	bsp_node_s* parent;
	bsp_node_s* front;
	bsp_node_s* back;

	private:
	list<polygon_t*> polygonList;
	list<entity_t*> entityList;


	public:

	// node helpers

	bool isLeaf()	{
		if(front)
			return false;

		return true;
	}


	// Encapsulate the polygon list so we aren't
	// tempted to modify it elsewhere.

	void addPolygon(polygon_t* poly)	{
		polygonList.push_back(poly);
	}

	void removePolygon(polygon_t* poly)	{
		polygonList.remove(poly);
	}

	void clearNode()	{
		if( partition )
			delete partition;

		int size = polygonList.size();
		for(int x=0; x < size; x++)	{
			polygon_t* poly = polygonList.front();
			if( poly )
				delete poly;
			polygonList.pop_front();
		}

		size = entityList.size();
		for(int x=0; x < size; x++)	{
			entity_t* ent = entityList.front();
			if( ent )
				delete ent;
			entityList.pop_front();
		}
	}

	list<polygon_t*>::iterator beginPolyListItr()	{
		return polygonList.begin();
	}

	list<polygon_t*>::iterator endPolyListItr()	{
		return polygonList.end();
	}

	list<polygon_t*> getPolygonList()	{
		return polygonList;
	}

	void setPolygonList(list<polygon_t*> polyList)	{
		polygonList = polyList;
	}

	int getPolygonCount()	{
		return polygonList.size();
	}

	void addEntity(entity_t* ent)	{
		entityList.push_back(ent);
	}

	list<entity_t*> getEntityList()	{
		return entityList;
	}

}bsp_node_t;

// splitPolygon is only used in bsptree.cpp, hence it's lack of visiblity here
bsp_node_t* getNewBSPNode();
void buildTree(bsp_node_t*);
void bspInOrderBackToFront(bsp_node_t* tree);
void bspInOrderFrontToBack(bsp_node_t* tree);
void deleteTree(bsp_node_t* bspRoot);
void generateBSPTree(bsp_node_t* root, list<polygon_t*> polygonList, float initialDiameter);
bsp_node_t* findBSPLeaf(bsp_node_t* bspRoot, const vec3_t pos);

#endif /* BSPTREE_H_ */
