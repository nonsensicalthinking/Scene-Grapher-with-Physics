/*
 * BSPTree.cpp
 *
 *  Created on: Jun 4, 2010
 *      Author: Derek Brooks
 */


/*
 * TODO LIST for this file
 *
 * Empty for now, but I'm certainly not done with this file yet.
 *
 */

#include <iostream>
#include <cmath>
#include <string.h>
#include "shared.h"
#include "bsptree.h"

//#define DEBUG_SPLIT	// for debugging the bsp partitions

using namespace std;

#define FRONT 		 1
#define BACK 		-1
#define COINCIDENT	 0

const vec3_t NORMAL_X_DIR = {1.0, 0.0, 0.0};
const vec3_t NORMAL_Z_DIR = {0.0, 0.0, 1.0};


//	splitPolygon is one of those long and messy functions that needs a
// 	good solid re-write, but because it works for now it will remain
//	in this state.  When we slice the polygon "poly" with the plane "split"
//	we also calculate if any, the intersection with the plane and any texture
// 	present on the polygon.  This has to be done in two different ways, there
//	is an if statement with an else if and an else, the two if statements are
//	nearly identical, but actually aren't.  Just a thought.
void splitPolygon(const polygon_t *poly, const plane_t *split, polygon_t *front, polygon_t *back)
{
	int x;
	int frontCount = 0, backCount = 0;

	vec3_t frontPoints[MAX_POLY_POINTS];
	vec2_t frontTexPoints[MAX_POLY_POINTS];
	vec3_t frontNormPoints[MAX_POLY_POINTS];

	vec3_t backPoints[MAX_POLY_POINTS];
	vec2_t backTexPoints[MAX_POLY_POINTS];
	vec3_t backNormPoints[MAX_POLY_POINTS];

	vec3_t pointA;
	vec3_t pointB;

	vec2_t texPointA;
	vec2_t texPointB;


	// Begin with the last point on the polygon
	VectorCopy(poly->points[poly->numPoints-1], pointA);
	if( poly->isTextured )
		VectorCopy(poly->texpts[poly->numPoints-1], texPointA);


	// Classify it (the last point on the polygon), and loop the
	// rest of the points looking for two points on opposite sides of the plane.
	float sideA = classifyPoint(split, pointA);
	float sideB;

	for(x=0; x < poly->numPoints; x++)	{

		VectorCopy(poly->points[x], pointB);
		if( poly->isTextured )
			VectorCopy2f(poly->texpts[x], texPointB);

		sideB = classifyPoint(split, pointB);


		if( sideB > EPSILON )	{
			if( sideA < -EPSILON )	{
				// compute the intersection point of the line
				// from point A to point B with the partition
				// plane. This is a ray-plane intersection.
				vec3_t intersection;
				float fractSect;

				if( (findLinePlaneIntersect(split, pointA, pointB, intersection, &fractSect)) )	{
					VectorCopy(intersection, frontPoints[frontCount]);
					VectorCopy(intersection, backPoints[backCount]);

					if( poly->isTextured )	{
						vec2_t work;
						vec2_t u;
						// Compute the texture intersection coords
						VectorSubtract2f(texPointB, texPointA, u);
						VectorMA2f(texPointA, u, fractSect, work);
						VectorCopy2f(work, frontTexPoints[frontCount]);
						VectorCopy2f(work, backTexPoints[backCount]);
					}

					if( poly->hasNormals )	{
						VectorCopy(poly->normpts[x], frontNormPoints[frontCount]);
						VectorCopy(poly->normpts[x], backNormPoints[backCount]);
					}

					frontCount++;
					backCount++;
				}
			}

			// These points are added to the FRONT no matter what
			VectorCopy(pointB, frontPoints[frontCount]);
			if( poly->isTextured )
				VectorCopy2f(texPointB, frontTexPoints[frontCount]);
			if( poly->hasNormals )
				VectorCopy(poly->normpts[x], frontNormPoints[frontCount]);

			// jack up the front count
			frontCount++;
		}
		else if( sideB < -EPSILON )	{
			if( sideA > EPSILON )	{
				// compute the intersection point of the line
				// from point A to point B with the partition
				// plane. This is a ray-plane intersection.
				vec3_t intersection;
				float fractSect;

				if( (findLinePlaneIntersect(split, pointA, pointB, intersection, &fractSect)) )	{
					VectorCopy(intersection, frontPoints[frontCount]);
					VectorCopy(intersection, backPoints[backCount]);

					if( poly->isTextured )	{
						vec2_t work;
						vec2_t u;
						// Compute the texture intersection coords
						VectorSubtract2f(texPointB, texPointA, u);
						VectorMA2f(texPointA, u, fractSect, work);
						VectorCopy2f(work, frontTexPoints[frontCount]);
						VectorCopy2f(work, backTexPoints[backCount]);
					}

					if( poly->hasNormals )	{
						VectorCopy(poly->normpts[x], frontNormPoints[frontCount]);
						VectorCopy(poly->normpts[x], backNormPoints[backCount]);
					}

					frontCount++;
					backCount++;
				}
			}

			// These points are added to the BACK no matter what
			VectorCopy(pointB, backPoints[backCount]);
			if( poly->isTextured )
				VectorCopy2f(texPointB, backTexPoints[backCount]);
			if( poly->hasNormals )
				VectorCopy(poly->normpts[x], backNormPoints[backCount]);


			// jack up the back count
			backCount++;
		}
		else	{	// coincident point
			VectorCopy(pointB, frontPoints[frontCount]);
			VectorCopy(pointB, backPoints[backCount]);

			if( poly->isTextured )	{
				VectorCopy2f(texPointB, frontTexPoints[frontCount]);
				VectorCopy2f(texPointB, backTexPoints[backCount]);
			}

			if( poly->hasNormals )	{
				VectorCopy(poly->normpts[x], frontNormPoints[frontCount]);
				VectorCopy(poly->normpts[x], backNormPoints[backCount]);
			}

			frontCount++;
			backCount++;

			// FIXME: this is only a warning cause I don't
			// know how it will react atm with coincidence
			cout << "BSP WARNING: COINCIDENT pointB: ";
			VectorPrint(pointB);
			cout << endl;
		}

		VectorCopy(pointB, pointA); 		// pointA = pointB
		VectorCopy2f(texPointB, texPointA);	// texPointA = texPointB
		sideA = sideB;						// iterate son!
	}

	// put the points in the new polygons
	front->numPoints = frontCount;
	for(x=0; x < frontCount; x++)	{
		VectorCopy(frontPoints[x], front->points[x]);
		if( poly->isTextured )
			VectorCopy(frontTexPoints[x], front->texpts[x]);
		if( poly->hasNormals )
			VectorCopy(frontNormPoints[x], front->normpts[x]);
	}

	back->numPoints = backCount;
	for(x=0; x < backCount; x++)	{
		VectorCopy(backPoints[x], back->points[x]);
		if( poly->isTextured )
			VectorCopy(backTexPoints[x], back->texpts[x]);
		if( poly->hasNormals )
			VectorCopy(backNormPoints[x], back->normpts[x]);
	}

}

bsp_node_t* getNewBSPNode()	{
	bsp_node_t* node = new bsp_node_t;

	node->root = false;
	node->partition = NULL;
	node->parent = NULL;
	node->front = NULL;
	node->back = NULL;

	return node;
}


void buildTree(bsp_node_t* currentNode)
{
	static int depth = 0;
	static int leafCount = 0;

	// FIXME: isn't tree depth zero based?
	depth++;

	if( depth == BSP_RECURSION_DEPTH )	{
		// TODO get dynamic object list from this node and insert into master listing of dynobjlists
		leafCount++;
		depth--;
		return;
	}
	else	{

		float sideOfPlane = 0;
		list<polygon_t*>::iterator itr;
		list<polygon_t*> front_list;
		list<polygon_t*> back_list;

		for(itr=currentNode->beginPolyListItr(); itr != currentNode->endPolyListItr(); itr++)	{
			polygon_t* curPoly = (*itr);

			int polySide = classifyPolygon(currentNode->partition, curPoly);

			if( polySide == SPANNING )	{
				polygon_t* front_half = createPolygon();
				polygon_t* back_half = createPolygon();

				// share properties from parent polygon.
				strcpy(front_half->materialName, curPoly->materialName);
				front_half->hasMaterial = curPoly->hasMaterial;
				front_half->hasNormals = curPoly->hasNormals;
				front_half->isTextured = curPoly->isTextured;

				strcpy(back_half->materialName, curPoly->materialName);
				back_half->hasMaterial = curPoly->hasMaterial;
				back_half->hasNormals = curPoly->hasNormals;
				back_half->isTextured = curPoly->isTextured;

				splitPolygon(curPoly, currentNode->partition, front_half, back_half);

				front_list.push_back(front_half);
				back_list.push_back(back_half);
			}
			else	{	// Polygon is on one side or the other
				if( polySide == FRONT )	{ // Front side of plane
					front_list.push_back(curPoly);
				}
				else	{// if( polySide == BACK )	// Back side of plane
					back_list.push_back(curPoly);
				}
			}
		}


		/*
		 * End polygon splitting, prepare for recursion
		 */


		// build new partitions
		vec3_t newFront;
		vec3_t newBack;
		plane_t* fp = new plane_t;
		plane_t* bp = new plane_t;
		bsp_node_t* front = getNewBSPNode();
		bsp_node_t* back = getNewBSPNode();

		VectorCopy(currentNode->partition->origin, newFront);
		VectorCopy(currentNode->partition->origin, newBack);

		if( currentNode->partition->normal[PLANE_NORMAL_X] == 1 )	{
			float diameter = currentNode->partition->diameter / 2;	// only modify diameter after Z split (previous split)

			newFront[PLANE_NORMAL_X] += diameter;
			newBack[PLANE_NORMAL_X] -= diameter;

			VectorCopy(newFront, fp->origin);
			VectorCopy(NORMAL_Z_DIR, fp->normal);
			fp->diameter = diameter;

			VectorCopy(newBack, bp->origin);
			VectorCopy(NORMAL_Z_DIR, bp->normal);
			bp->diameter = diameter;

		}	else	{	// PLANE_NORMAL_Z
			newFront[PLANE_NORMAL_Z] += currentNode->partition->diameter;
			newBack[PLANE_NORMAL_Z] -= currentNode->partition->diameter;

			VectorCopy(newFront, fp->origin);
			VectorCopy(NORMAL_X_DIR, fp->normal);
			fp->diameter = currentNode->partition->diameter;

			VectorCopy(newBack, bp->origin);
			VectorCopy(NORMAL_X_DIR, bp->normal);
			bp->diameter = currentNode->partition->diameter;
		}

		// Give the nodes work to do
		front->setPolygonList(front_list);
		back->setPolygonList(back_list);

		front->partition = fp;
		back->partition = bp;

		currentNode->front = front;
		currentNode->back = back;

		front->parent = currentNode;
		back->parent = currentNode;

		buildTree(front);
		buildTree(back);
	}

	depth--;
}


void bspInOrderBackToFront(bsp_node_t* tree)	{

	if( !tree )	{
		cout << "WARNING BSPTree: Null Tree Reference" << endl;
		return;
	}

	if( tree->isLeaf() )	{
		cout << "Found a leaf!" << endl;
		return;
	}

	bspInOrderBackToFront(tree->back);
	bspInOrderBackToFront(tree->front);
}

void bspInOrderFrontToBack(bsp_node_t* tree)	{
	if( !tree )	{
		cout << "WARNING BSPTree: Null Tree Reference" << endl;
		return;
	}

	if( tree->isLeaf() )	{
		cout << "Found a leaf!" << endl;
		cout << "Polygons: " << tree->getPolygonList().size() << endl;
		return;
	}

	bspInOrderFrontToBack(tree->front);
	bspInOrderFrontToBack(tree->back);
}

void deleteTree(bsp_node_t* tree)	{


	if( !tree )	{
		cout << "Warning BSPTree: Null tree reference" << endl;
		return;
	}

	cout << "Tree Node: " << tree << endl;


	if( tree->root )	{
		cout << "THIS IS THE ROOT" << endl;
	}
	else if( tree->isLeaf() )	{
		tree->clearNode();
	}
	else	{
		deleteTree(tree->front);
		deleteTree(tree->back);
		if( tree->partition )
			delete tree->partition;
	}

	delete tree;
}


void generateBSPTree(bsp_node_t* root, list<polygon_t*> polygonList, float initialDiameter)	{

	plane_t* partition = new plane_t;

	partition->normal[0] = 1.0;
	partition->normal[1] = 0.0;
	partition->normal[2] = 0.0;

	partition->origin[0] = 0.0;
	partition->origin[1] = 0.0;
	partition->origin[2] = 0.0;

	partition->diameter = initialDiameter;

	root->setPolygonList(polygonList);
	root->partition = partition;

	// the first argument must be double the width of the largest surface
	buildTree(root);
	// FIXME THIS WAS 240 for FENWAY, SHOULD BE VARIABLE
	// ITS 120 NOW FOR A TEST
}

// TODO make this return a leaf list when doing sphere and bounding box collisions
// because the box or sphere may be laying in more than one partition
bsp_node_t* findBSPLeaf(bsp_node_t* bspRoot, const vec3_t pos)	{
	bsp_node_t* curNode = bspRoot;

	while( !curNode->isLeaf() )	{
		if( curNode->partition != NULL )	{
			float result = classifyPoint(curNode->partition, pos);
			if( result > EPSILON )	{
				curNode = curNode->front;
			}
			else if(result < -EPSILON )	{
				curNode = curNode->back;
			}
			else	{
				//TODO HANDLE SPANNING OCCURANCE
				cout << "Ent position spanning two nodes" << endl;
				//FIXME for now just going down the front
				// Will need to go down both front and back!
				curNode = curNode->front;
			}
		}
		else	{
			cout << "NULL NODE REFERENCE" << endl;
		}
	}

	if( curNode->isLeaf() )
		return curNode;

	return NULL;
}

/*

// Very simple BSP Test

int main(void)
{
	list<polygon_t*> polygonList;

	polygon_t* p = new polygon_t;
	p->numPoints = 4;

	p->points[0][0] = -199;
	p->points[0][1] = 0;
	p->points[0][2] = -199;

	p->points[1][0] = -199;
	p->points[1][1] = 0;
	p->points[1][2] = 200;

	p->points[2][0] = 200;
	p->points[2][1] = 0;
	p->points[2][2] = 200;

	p->points[3][0] = 200;
	p->points[3][1] = 0;
	p->points[3][2] = -199;

	polygonList.push_back(p);

	bsp_node_t* root = new bsp_node_t;


	plane_t* partition = new plane_t;

	partition->normal[0] = 1.0;
	partition->normal[1] = 0.0;
	partition->normal[2] = 0.0;

	partition->origin[0] = 0.0;
	partition->origin[1] = 0.0;
	partition->origin[2] = 0.0;

	root->setPolygonList(polygonList);


	buildTree(400, 0, partition, root);

	bspInOrderFrontToBack(root);

	deleteTree(root);

	return 0;
}

*/




/*
 * This function tests the working-ness of splitting polygons
 * oh, but you have to evaluate the results by paper =P
 */

/*
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
*/














