/*
 * BSPTree.cpp
 *
 *  Created on: Jun 4, 2010
 *      Author: brof
 */

#include <iostream>
#include <cmath>
#include "shared.h"
#include "BSPTree.h"


using namespace std;

#define FRONT 		1
#define BACK 		-1
#define COINCIDENT	0




void splitPolygon(const polygon_t *poly, const plane_t *split, polygon_t *front, polygon_t *back)
{
	int frontCount = 0, backCount = 0;
	int x;
	vec3_t frontPoints[MAX_POLY_POINTS];
	vec3_t backPoints[MAX_POLY_POINTS];

	vec3_t pointA;
	vec3_t pointB;

	VectorCopy(poly->points[poly->numPoints-1], pointA);

	float sideA = classifyPoint(split, pointA);
	float sideB;

	cout << "Num Points: " << poly->numPoints << endl;

	for(x=0; x < poly->numPoints; x++)	{
		VectorCopy(poly->points[x], pointB);
		sideB = classifyPoint(split, pointB);

		cout << "Working on pointA " << x << ": [" << pointA[0] << ", " << pointA[1] << ", " << pointA[2] << "]" << endl;
		cout << "Working on pointB " << x << ": [" << pointB[0] << ", " << pointB[1] << ", " << pointB[2] << "]" << endl;
		cout << "sideA: " << sideA << endl;
		cout << "sideB: " << sideB << endl;

		if( sideB > 0 )	{
			if( sideA < 0 )	{
				cout << "Line crosses plane." << endl;
				// compute the intersection point of the line
				// from point A to point B with the partition
				// plane. This is a simple ray-plane intersection.

				vec3_t intersection;

				// split in this def is of type plane_g (a struct with plane info)
				if( (findLinePlaneIntersect(split, pointA, pointB, intersection )) )	{
					VectorCopy(intersection, frontPoints[frontCount++]);
					VectorCopy(intersection, backPoints[backCount++]);
				}
			}
			else
				cout << "Same side of plane." << endl;

			VectorCopy(pointB, frontPoints[frontCount++]);
		}
		else if( sideB < 0 )	{
			if( sideA > 0 )	{
				cout << "Line crosses plane." << endl;
				// compute the intersection point of the line
				// from point A to point B with the partition
				// plane. This is a simple ray-plane intersection.

				vec3_t intersection;

				if( (findLinePlaneIntersect(split, pointA, pointB, intersection )) )	{
					VectorCopy(intersection, frontPoints[frontCount++]);
					VectorCopy(intersection, backPoints[backCount++]);
				}
			}
			else
				cout << "Same side of plane." << endl;

			VectorCopy(pointB, backPoints[backCount++]);
		}
		else	{	// coincident ?
			VectorCopy(pointB, frontPoints[frontCount++]);
			VectorCopy(pointB, backPoints[backCount++]);
			// FIXME: this is only a warning cause I don't
			// know how it will react atm with coincidence
			cout << "WARNING: COINCIDENT pointB: ";
			VectorPrint(pointB);
			cout << endl;
		}

		VectorCopy(pointB, pointA); // pointA = pointB
		sideA = sideB;
	}

	cout << "Front Point Count: " << frontCount << endl;
	cout << "Back Point Count: " << backCount << endl;

	front->numPoints = frontCount;
	for(x=0; x < frontCount; x++)	{
		VectorCopy(frontPoints[x], front->points[x]);

		cout << "front->points: ";
		VectorPrint(front->points[x]);
		cout << endl;
	}

	back->numPoints = backCount;
	for(x=0; x < backCount; x++)	{
		VectorCopy(backPoints[x], back->points[x]);
		cout << "back->points: ";
		VectorPrint(back->points[x]);
		cout << endl;
	}

}


void buildTree(const float width, const float height, plane_t* partition, bsp_node_t* this_node)
{
	static int depth = 0;

	depth++;

	if( depth >= BSP_RECURSION_DEPTH )	{
		cout << "Level " << depth << ": " << "END OF BRANCH (LEAF)" << endl;
		depth--;
		return;
	}
	else	{
		cout << "Level " << depth << ": " << "Splitting polygon list..." << endl;

		this_node->partition = partition;

		float sideOfPlane = 0;
		list<polygon_t*>::iterator itr;
		list<polygon_t*> front_list;
		list<polygon_t*> back_list;

		cout << "Level " << depth << "Polygon count: " << this_node->getPolygonCount() << endl;

		for(itr=this_node->beginPolyListItr(); itr != this_node->endPolyListItr(); itr++)	{
			polygon_t* curPoly = (*itr);

			int polySide = classifyPolygon(partition, curPoly);

			if( polySide == SPANNING )	{

				cout << "Polygon is SPANNING partition" << endl;

				polygon_t* front_half = new polygon_t;
				polygon_t* back_half = new polygon_t;

				splitPolygon(curPoly, partition, front_half, back_half);

				front_list.push_back(front_half);
				back_list.push_back(back_half);
			}
			else	{	// Polygon is on one side or the other
				if( polySide == FRONT )	{ // Front side of plane
					cout << "Polygon in front of partition" << endl;
					front_list.push_back(curPoly);
				}
				else	{// if( polySide == BACK )	// Back side of plane
					cout << "Polygon in back of partition" << endl;
					back_list.push_back(curPoly);
				}
			}
		}

		cout << "Level " << depth << ": " << "Done splitting polygons" << endl;

		/*
		 * Begin calculation of next partitioning plane
		 */
		plane_t* new_front_partition = new plane_t;
		plane_t* new_back_partition = new plane_t;

		VectorCopy(partition->origin, new_front_partition->origin);
		VectorCopy(partition->origin, new_back_partition->origin);

		float newWidth;
		float newHeight;
		float nextCenter;

		if( partition->normal[PLANE_NORMAL_X] == 0.0 )	{
			// if the last partition was on the z axis
			// make this one on the x axis
			newWidth = width;
			newHeight = width / 2;
			nextCenter = newHeight / 2;

			new_front_partition->origin[PLANE_NORMAL_Z] -= nextCenter;
			new_back_partition->origin[PLANE_NORMAL_Z] += nextCenter;

			VectorCopy(NORMAL_X, new_front_partition->normal);
			VectorCopy(NORMAL_X, new_back_partition->normal);

			cout << "Next Partition Normal: ";
			VectorPrint(new_front_partition->normal);
			cout << endl;

			cout << "Next Partition Origin: ";
			VectorPrint(new_front_partition->origin);
			cout << endl;

			cout << "newWidth = " << newWidth << endl;
			cout << "newHeight = " << newHeight << endl;
			cout << "Next Center = " << nextCenter << endl;

		}
		else	{	// partition->normal[PLANE_NORMAL_Z] == 0.0
			// the last partition was on the x axis
			// make this one on the z axis
			newWidth = height / 2;
			newHeight = height;
			nextCenter = newWidth / 2;

			new_front_partition->origin[PLANE_NORMAL_X] -= nextCenter;
			new_back_partition->origin[PLANE_NORMAL_X] += nextCenter;

			VectorCopy(NORMAL_Z, new_front_partition->normal);
			VectorCopy(NORMAL_Z, new_back_partition->normal);

			cout << "Next Partition Normal: ";
			VectorPrint(new_front_partition->normal);
			cout << endl;

			cout << "Next Partition Origin: ";
			VectorPrint(new_front_partition->origin);
			cout << endl;

			cout << "newWidth = " << newWidth << endl;
			cout << "newHeight = " << newHeight << endl;
			cout << "Next Center = " << nextCenter << endl;

		}

		/*
		 * End creation of new partitioning planes
		 */

		cout << "Level " << depth << ": " << "Done creating new partitions" << endl;

		bsp_node_t* front_node = new bsp_node_t;
		bsp_node_t* back_node = new bsp_node_t;

		// Link the new nodes to the tree
		front_node->parent = this_node;
		back_node->parent = this_node;

		this_node->front = front_node;
		this_node->back = back_node;
		// End node linking

		// Give the nodes work to do
		front_node->setPolygonList(front_list);
		back_node->setPolygonList(back_list);

		cout << "Level " << depth << ": " << "Done linking, calling next FRONT" << endl;
		buildTree(newWidth, newHeight, new_front_partition, front_node);

		cout << "Level " << depth << ": " << "Done linking, calling next BACK" << endl;
		buildTree(newWidth, newHeight, new_back_partition, back_node);
	}

	depth--;
}




int main(void)
{
	list<polygon_t*> polygonList;

	polygon_t* p = new polygon_t;
	p->numPoints = 4;

	p->points[0][0] = -175;
	p->points[0][1] = 0;
	p->points[0][2] = -175;

	p->points[1][0] = 175;
	p->points[1][1] = 0;
	p->points[1][2] = -175;

	p->points[2][0] = 175;
	p->points[2][1] = 0;
	p->points[2][2] = 175;

	p->points[3][0] = -175;
	p->points[3][1] = 0;
	p->points[3][2] = 175;

	polygonList.push_back(p);

	bsp_node_t* root = new bsp_node_t;
	plane_t* partition = new plane_t;

	partition->normal[0] = 0.0;
	partition->normal[1] = 0.0;
	partition->normal[2] = 1.0;

	partition->origin[0] = 0.0;
	partition->origin[1] = 0.0;
	partition->origin[2] = 0.0;

	root->setPolygonList(polygonList);
	root->partition = partition;

	buildTree(400, 400, partition, root);

	return 0;
}





BSPTree::BSPTree() {
	// TODO Auto-generated constructor stub

}

BSPTree::~BSPTree() {
	// TODO Auto-generated destructor stub
}




/*
 * This func tests the working-ness of splitting polygons
 * oh, but you have to evaluate the results by paper =P
 */
void testSplittingOperations()
{
	int x;

	polygon_t *poly = new polygon_t;
	polygon_t *front = new polygon_t;
	polygon_t *back = new polygon_t;

	plane_t *splitPlane = new plane_t;


	splitPlane->origin[0] = 2.0;
	splitPlane->origin[1] = 0.0;
	splitPlane->origin[2] = 3.0;

	splitPlane->normal[0] = 0.0;
	splitPlane->normal[1] = 0.0;
	splitPlane->normal[2] = 1.0;

	cout << "===> Splitting square..." << endl;

	poly->numPoints = 4;
	vec3_t polygon[] = {{1.0f, 0.0f, 2.0f},
						{1.0f, 0.0f, 4.0f},
						{3.0f, 0.0f, 4.0f},
						{3.0f, 0.0f, 2.0f}};

	for(x=0; x < poly->numPoints; x++)
		VectorCopy(polygon[x], poly->points[x]);

	splitPolygon(poly, splitPlane, front, back);

	for(x =0; x < front->numPoints; x++)	{
		cout << "Front Point " << x << ": ";
		VectorPrint(front->points[x]);
		cout << endl;
	}

	for(x =0; x < back->numPoints; x++)	{
		cout << "Back Point " << x << ": ";
		VectorPrint(back->points[x]);
		cout << endl;
	}

	// Triangle
	cout << "===> Splitting triangle..." << endl;
	delete poly;
	poly = new polygon_t;

	vec3_t polygonB[] = {{1.0, 0.0, 1.0}, {2.5, 0.0, 4.0}, {4.0, 0.0, 1.0} };

	splitPlane->origin[0] = 2.5;
	splitPlane->origin[1] = 0.0;
	splitPlane->origin[2] = 2.0;

	splitPlane->normal[0] = 0.0;
	splitPlane->normal[1] = 0.0;
	splitPlane->normal[2] = 1.0;

	poly->numPoints = 3;

	for(x=0; x < poly->numPoints; x++)
		VectorCopy(polygonB[x], poly->points[x]);

	splitPolygon(poly, splitPlane, front, back);

	for(x =0; x < front->numPoints; x++)	{
		cout << "Front Point " << x << ": ";
		VectorPrint(front->points[x]);
		cout << endl;
	}

	for(x =0; x < back->numPoints; x++)	{
		cout << "Back Point " << x << ": ";
		VectorPrint(back->points[x]);
		cout << endl;
	}

}















