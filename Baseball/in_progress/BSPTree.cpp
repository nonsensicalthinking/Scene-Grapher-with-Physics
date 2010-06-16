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

#define EPSILON		0.0001f


float classifyPoint(const plane_t *plane, const vec3_t point)
{
	return DotProduct(plane->normal, point) - DotProduct(plane->normal, plane->origin);
}


int findLinePlaneIntersect( const plane_t *plane, const vec3_t pointA, const vec3_t pointB, vec3_t intersect )
{
	vec3_t u;
	vec3_t w;

	VectorSubtract(pointB, pointA, u);
	VectorSubtract(pointA, plane->origin, w);

	float numerator = -DotProduct(plane->normal, w);
	float denominator = DotProduct(plane->normal, u);

	if (fabs(denominator) < EPSILON) {          // segment is parallel to plane
		if (numerator == 0)                     // segment lies in plane
			return 2;
		else
			return 0;                   // no intersection
	}

	// they are not parallel
	// compute intersect param
	float fractSect = numerator / denominator;

	if( fractSect < 0 || fractSect > 1 )
		return 0;                       // no intersection

	VectorMA( pointA, u, fractSect, intersect);

	return 1;	// Indicate that we had an intersection
}





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

	cout << "------------Polygon Split--------------" << endl;
	cout << "Num Points: " << poly->numPoints << endl;

	for(x=0; x < poly->numPoints; x++)	{
		VectorCopy(poly->points[x], pointB);
		sideB = classifyPoint(split, pointB);

//		cout << "Working on pointA " << x << ": [" << pointA[0] << ", " << pointA[1] << ", " << pointA[2] << "]" << endl;
//		cout << "Working on pointB " << x << ": [" << pointB[0] << ", " << pointB[1] << ", " << pointB[2] << "]" << endl;
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

	cout << "---------------Polygon Split Complete---------------" << endl;

}


void initPoly(polygon_t* poly)	{
	int x=0;
	poly->numPoints = 0;

	for(x=0; x < MAX_POLY_POINTS; x++)
		VectorInit(poly->points[x]);
}


/*
void buildBSPTree(bsp_node_t* tree, polygon_t* polygons, const plane_t* partition, int depth)
{
   polygon   *root = polygons.Get_From_List ();
   tree->partition = partition;
   tree->addPolygon(root);

   polygon_t* front_list;
   polygon_t* back_list;

   polygon_t* poly;

   int x;
   for(x=0; x <  )


   while ((poly = polygons.Get_From_List ()) != 0)
   {
      int   result = tree->partition.Classify_Polygon (poly);
      switch (result)
      {
         case COINCIDENT:
            tree->polygons.Add_To_List (poly);
            break;
         case IN_BACK_OF:
            back_list.Add_To_List (poly);
            break;
         case IN_FRONT_OF:
            front_list.Add_To_List (poly);
            break;
         case SPANNING:
            polygon   *front_piece, *back_piece;
            Split_Polygon (poly, tree->partition, front_piece, back_piece);
            back_list.Add_To_List (back_piece);
            front_list.Add_To_List (front_piece);
            break;
      }
   }
   if ( ! front_list.Is_Empty_List ())
   {
      tree->front = new BSP_tree;
      Build_BSP_Tree (tree->front, front_list);
   }
   if ( ! back_list.Is_Empty_List ())
   {
      tree->back = new BSP_tree;
      Build_BSP_Tree (tree->back, back_list);
   }
}
*/

/*
 * This main tests the working-ness of splitting polygons
 */

int main( void )
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

	splitPlane->normal[0] = 1.0;
	splitPlane->normal[1] = 0.0;
	splitPlane->normal[2] = 0.0;

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





	return 0;
}


#define BSP_RECURSION_DEPTH		4
#define		PLANE_NORMAL_X		0
#define 	PLANE_NORMAL_Y		1
#define		PLANE_NORMAL_Z		2
#define		BACK				-1
#define		SPANNING			0
#define		FRONT				1

const vec3_t NORMAL_X = {1.0, 0.0, 0.0};
const vec3_t NORMAL_Y = {0.0, 1.0, 0.0};
const vec3_t NORMAL_Z = {0.0, 0.0, 1.0};



int classifyPolygon(const plane_t* partition, const polygon_t* poly)	{

	int x;
	bool hasFront = false;
	bool hasBack = false;


	for(x=0; x < poly->numPoints; x++)	{
		float classification = classifyPoint(partition, poly->points[x]);

		// we can do the returns below because if any point on the
		// polygon is on the opposite side there will be a split
		// we don't care to check every point, the splitting routines
		// will do that for us later.
		if( classification  >= 0 )	{
			hasFront = true;

			if( hasBack )
				return SPANNING;

		}
		else	{ // if( classification < 0 )
			hasBack = true;

			if( hasFront )
				return SPANNING;
		}
	}


	if( hasFront )
		return FRONT;

	if( hasBack )
		return BACK;

	return -99;	// Error of sorts happened.
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



/*




int main(void)
{
	list<polygon_t*> polygonList;

	polygon_t* p = new polygon_t;
	p->numPoints = 4;

	p->points[0][0] = -175;
	p->points[0][1] = 0;
	p->points[0][2] = -175;

	p->points[0][0] = 175;
	p->points[0][1] = 0;
	p->points[0][2] = -175;

	p->points[0][0] = 175;
	p->points[0][1] = 0;
	p->points[0][2] = 175;

	p->points[0][0] = -175;
	p->points[0][1] = 0;
	p->points[0][2] = 175;

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







/*

int main(void)
{
	int x;

	bsp_node_t* node = new bsp_node_t;

	for(x=0; x < 100; x++)	{
		polygon_t *a = new polygon_t;
		a->numPoints = x;
		node->addPolygon(a);
	}

	polygon_t* polyItr;

	for(polyItr=node->getPolygonList(); polyItr != NULL; polyItr=polyItr->nextPoly)
		cout << "Polygon number: " << polyItr->numPoints << endl;

	polygon_t* dpoly = node->getPolyByIndex(5);
	polygon_t* test = node->removePolygon(dpoly);


	if( test == dpoly )
		cout << "Same polygon returned from remove as we specified" << endl;
	else
		cout << "remove polygon removed a different poly than ours" << endl;

	delete dpoly;

	cout << "here" << endl;

	for(polyItr=node->getPolygonList(); polyItr != NULL; polyItr=polyItr->nextPoly)
		cout << "Polygon number: " << polyItr->numPoints << endl;



	return 0;
}*/




BSPTree::BSPTree() {
	// TODO Auto-generated constructor stub

}

BSPTree::~BSPTree() {
	// TODO Auto-generated destructor stub
}
