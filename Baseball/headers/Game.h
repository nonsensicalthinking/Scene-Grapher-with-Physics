/*
 * Game.cpp
 *
 *  Created on: Jul 26, 2010
 *      Author: brof
 */

#include "Scene.h"
#include "physics.h"
#include <iostream>
#include <queue>

using namespace std;

extern Scene* getScene();
extern long timeStamp;


class Game	{

private:
	bool gameOn;

protected:
	bsp_node_t* bspRoot;
	list<Simulation*>	dynamicObjects;

public:
	float timeElapsed;
	float slowMotionRatio;
	list<entity_t*> entityList;
	list<list<entity_t*> > entityLeafList;
	queue<entity_t*> unusedEnts;
	bool loaded;


	Game()	{

	}

	~Game()	{

	}

	void killGame()	{
		gameOn = false;
	}

	// Run will call load, this is where the maps are loaded from
	void run()	{
/*
//		load();
		gameOn = true;
		long preTime = 0;
		long curTime = timeStamp;

		while( gameOn )	{
			preTime = curTime;
			curTime = timeStamp;
			long dt = curTime - preTime;

			advance(dt);
		}

		cout << "Game thread ended." << endl;
*/
	}

	// X PINCH POINT X
	// not thread safe
	virtual void recycleEntity(entity_t* ent)	{
		cleanEntity(ent);
		unusedEnts.push(ent);
	}

	// you're responsible for getting one...
	// this reference may or may not come back NULL
	virtual entity_t* getFreshEntity()	{
		entity_t* ent = unusedEnts.front();
		unusedEnts.pop();
		return ent;
	}



	// Event handlers
	void keyPressedEvent(unsigned char key, int x, int y)	{
		Scene* curScene = getScene();

		if( curScene->con->consoleActive )	{	// send key input to console
			switch(key)	{
				case CONSOLE_KEY:	// deactivate console
						curScene->con->consoleActive = !curScene->con->consoleActive;
					break;
				case ESC_KEY:
					curScene->con->clearInput();
					break;
				default:	// add to input line
					curScene->con->appendToInput(key);
			}
		}
		else	{	// Don't send key input to console
			keyPressed(key, x, y);
		}
	}

	void specialKeyPressedEvent(int key, int x, int y)	{
		Scene* curScene = getScene();

		if( curScene->con->consoleActive )	{	// send key input to console
			switch(key)	{
				case F1_KEY:
					break;
				case F2_KEY:
					break;
				case F3_KEY:
					break;
				case F4_KEY:
					break;
				case F5_KEY:
					break;
				case F6_KEY:
					break;
				case F7_KEY:
					break;
				case F8_KEY:
					break;
				case F9_KEY:
					break;
				case F10_KEY:
					break;
				case F11_KEY:
					break;
				case F12_KEY:
					break;
				case ARROW_UP_KEY:
					break;
				case ARROW_DOWN_KEY:
					break;
				case PAGE_UP_KEY:
					curScene->con->scrollUp();
					break;
				case PAGE_DOWN_KEY:
					curScene->con->scrollDown();
					break;
				case HOME_KEY:
					break;
				case END_KEY:
					break;
				case INSERT_KEY:
					break;
				case NUM_LOCK_KEY:
					break;

			}
		}
		else	{
			specialKeyPressed(key, x, y);
		}
	}


	virtual void entPolyCollision(entity_t* ent, polygon_t* poly)	{
		switch( ent->collisionType )	{
		case COLLISION_NONE:	// You said it chief
			break;
		case COLLISION_BOX:		// Bounding Box vs Polygon
			// TODO perform BOX + POLY COLLISION
			break;
		case COLLISION_SPHERE:	// Sphere vs Polygon

			if(ent->mass->moveType == MOVE_TYPE_AT_REST)
				break;

			plane_t* plane = new plane_t;

			if( poly->hasNormals )	{
				VectorCopy(poly->normpts[0], plane->normal);
				VectorCopy(poly->points[0], plane->origin);

				vec3_t intersect;
				vec3_t rayEnd;

				VectorAdd(ent->mass->pos, ent->mass->vel, rayEnd);

				float discard;
				if( findLinePlaneIntersect(plane, ent->mass->pos, rayEnd, intersect, &discard) )	{
					if( isPointInPolygon(poly, intersect) )	{
						vec3_t dist;
						VectorSubtract(ent->mass->pos, intersect, dist);
						float distSquared = DotProduct(dist, dist);
						float rad = ent->radius + 0;	// no radius for polygon

						if( distSquared <= (rad*rad) )	{	// is it within striking distance?
							// TODO issue collision event so the engine can record what happened?
							vec3_t reflect;
							vec3_t incident;

							float len = VectorUnitVector(ent->mass->vel, incident); // the changed
							float newSpeed = len * 0.33;	// Shrink by 1/3~

							// FIXME stop rotating when we hit objects
							// FIXME not exactly accurate but ok for now
							ent->mass->rotationSpeed = 0;

							if( newSpeed < 0.1 )	{	// stay still
								ent->mass->moveType = MOVE_TYPE_AT_REST;
							}
							else	{	// reflect damn you!
								VectorReflect(incident, poly->normpts[0], reflect);
								VectorScale(reflect, newSpeed, ent->mass->vel);
								VectorCopy(ent->mass->prevPos, ent->mass->pos);
							}
						}
					}
				}
			}
			else	{	// Polygon has no normals... planeFromPoints()?
				// TODO Figure out how to handle this
			}

			break;
		}

	}

	virtual void entEntCollision(entity_t* ent, entity_t* otherEnt)	{
		if( !ent || !otherEnt )
			return;

		char collisionFlags = ent->collisionType | otherEnt->collisionType;
		switch(collisionFlags)	{
		case COLLISION_BOX|COLLISION_BOX:
			cout << "Implement Box v Box Collision" << endl;
			break;

		case COLLISION_SPHERE|COLLISION_SPHERE:
//			cout << "Implement Sphere v Sphere Collision" << endl;
			break;

		case COLLISION_CYLINDER|COLLISION_CYLINDER:
			cout << "Implement Cylinder v Cylinder Collision" << endl;
			break;

		case COLLISION_SPHERE|COLLISION_BOX:
			cout << "Implement Sphere v Box Collision" << endl;
			break;

		case COLLISION_SPHERE|COLLISION_CYLINDER:
//			cout << "Implement Sphere v Cylinder Collision" << endl;
			entity_t* ball;
			entity_t* bat;

			if( ent->collisionType == COLLISION_SPHERE )	{
				ball = ent;
				bat = otherEnt;
			}
			else	{
				ball = otherEnt;
				bat = ent;
			}

			vec3_t result, colPath, out;

			VectorSubtract(ball->mass->pos, bat->mass->pos, result);
			CrossProduct(ball->mass->vel, bat->cylinder->centerAxis, colPath);

			double length;
			length = VectorLength(colPath);
			double t;
			double s;
			double i, o;
			double lamda;

			if( (length < 0) && (length > -0) )
				return;

			VectorUnitVector(colPath, colPath);

			double delta;
			delta = fabs(DotProduct(result, colPath));

			if( delta <= bat->cylinder->radius )	{
				cout << "Ball hit bat!\a" << endl;

				CrossProduct(result, bat->cylinder->centerAxis, out);
				t = - DotProduct(out, colPath)/length;
				CrossProduct(colPath, bat->cylinder->centerAxis, out);
				VectorUnitVector(out, out);
				s = fabs(FastSqrt(bat->cylinder->radius*bat->cylinder->radius - delta*delta) / DotProduct(ball->mass->vel, out) );
				i = t - s;
				o = t + s;

				if(i < -0){
					if(o < -0)
						return;
					else
						lamda=o;
				}
				else if(o) {
					lamda=i;
				}
				else if(i < o)
					lamda=i;
				else
					lamda=o;

		    	vec3_t newposition;

		    	VectorSubtract(bat->mass->pos, ball->mass->vel, newposition);
		    	VectorUnitVector(newposition, newposition);
		    	VectorReflect(ball->mass->vel, newposition, newposition);
		    	VectorScale(newposition, ball->mass->instantSpeed, ball->mass->vel);
		    	VectorCopy(ball->mass->prevPos, ball->mass->pos);
		    	// to reflect
//		    	VectorReflect(incident, poly->normpts[0], reflect);
//				VectorScale(reflect, newSpeed, ent->mass->vel);


			}


			break;

		case COLLISION_CYLINDER|COLLISION_BOX:
			cout << "Implement Cylinder v Box Collision" << endl;
			break;
		}

	}

	virtual void collideWithWorld(entity_t* ent, list<polygon_t*> polyList)	{
		list<polygon_t*>::iterator itr = polyList.begin();
		for(; itr != polyList.end(); itr++)	{
			entPolyCollision(ent, (*itr));
		}
	}

	virtual void collideWithOtherEntities(entity_t* ent, list<entity_t*> entList)	{
		list<entity_t*>::iterator itr = entList.begin();
		for(; itr != entList.end(); itr++)	{
			if( ent != (*itr) )	// if its not itself
				entEntCollision(ent, (*itr));
		}
	}

	virtual void insertEntityIntoBSP(entity_t* ent)	{
		bsp_node_t* node = findBSPLeaf(bspRoot, ent->mass->pos);

		collideWithWorld(ent, node->getPolygonList());

		collideWithOtherEntities(ent, node->getEntityList());

		node->addEntity(ent);
	}

	virtual bsp_node_t* getBSPTree()	{
		return bspRoot;
	}

	virtual void unloadBSP()	{
		bspRoot = NULL;
		loaded = false;
	}

	virtual list<entity_t*> getEntList()	{
		return entityList;
	}

	virtual void createDynamicLeafList(bsp_node_t* root, bool start)	{
		if( start )
			entityLeafList.clear();

		if( root->isLeaf() )	{
			entityLeafList.push_back(root->getEntityList());
		}
		else	{
			createDynamicLeafList(root->front, false);
			createDynamicLeafList(root->back, false);
		}
	}

	virtual void removeEntitiesFromBSPTree()	{
		list<list<entity_t*> >::iterator itr;
		for(itr=entityLeafList.begin(); itr != entityLeafList.end(); itr++)
			(*itr).clear();
	}

	virtual void addEntity(entity_t* ent)	{
		entityList.push_back(ent);
	}

	virtual void removeEntity(entity_t* ent)	{
		entityList.remove(ent);
	}


	// Function stubs from here down.

	// This is to be overridden
	virtual void advance(float dt)	{

	}

	// This is to be overridden
	virtual void load(string mapname)	{

	}

	// This is to be overridden
	virtual void keyPressed(unsigned char key, int x, int y)	{

	}

	// This is to be overridden
	virtual void specialKeyPressed(int key, int x, int y)	{

	}

	// This is to be overridden
	virtual void mouseEvent(int button, int state, int x, int y)	{

	}

	// This is to be overridden
	virtual void passiveMouseEvent(int x, int y)	{

	}

};
