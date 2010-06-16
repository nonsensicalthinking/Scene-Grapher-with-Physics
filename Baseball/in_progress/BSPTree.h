/*
 * BSPTree.h
 *
 *  Created on: Jun 4, 2010
 *      Author: brof
 */

#ifndef BSPTREE_H_
#define BSPTREE_H_
#include <list>

// TODO: Consider making BSPNode class instead
typedef struct bsp_node_s	{

	plane_t *partition;
	bsp_node_s *parent;
	bsp_node_s *front;
	bsp_node_s *back;

	private:
	list<polygon_t*> polygonList;


	public:

	// node helpers

	bool isLeaf()	{
		if(!front)
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

	list<polygon_t*>::iterator beginPolyListItr()	{
		return polygonList.begin();
	}

	list<polygon_t*>::iterator endPolyListItr()	{
		return polygonList.end();
	}

	void setPolygonList(list<polygon_t*> polyList)	{
		polygonList = polyList;
	}

	int getPolygonCount()	{
		return polygonList.size();
	}

}bsp_node_t;




class BSPTree {
private:
	bsp_node_t* root;


public:
	BSPTree();
	virtual ~BSPTree();

//	void constructTree(polygon_t* polyList, );






};







#endif /* BSPTREE_H_ */
