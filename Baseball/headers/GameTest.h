/*
 * GameTest.h
 *
 *  Created on: Jul 26, 2010
 *      Author: brof
 *
 *
 *  TODO This particular todo may belong in the Game class but oh well...
 *  TODO Continued: Make it so resources can be unloaded and reloaded (vid_restart style)
 *
 *	The game tells the renderer what dynamic objects are in the scene.
 *  the game will refer it to as an entity.  Entities have masses that can be worked
 *	on and flags, possibly some AI by way of a think() system? Entities also have
 *	collision properties, sphere, cylinder, bounding box.
 *
 *	Collision detection will chick dynamic objects against the objects in the
 *	bsp area they're located in.
 *
 *
 *	TODO Check for redundancies in collision checks for entities
 *
 */
#include "Game.h"
#include "Scene.h"
#include "objloader.h"
#include "shared.h"

#define CAM_MOVE_RATE 	1


#define SKY_TEXTURE		"partly_cloudy.bmp"

// TODO REMOVE THIS
extern int pitchSpeed;

const vec3_t GRAVITY_EARTH = {0.0f, -9.81f, 0.0f};


// inherit the game class
class SpecialGame : public Game	{

public:

	bsp_node_t* bspRoot;
	list<entity_t*> entityList;
	list<list<entity_t*> > entityLeafList;

	float timeElapsed;
	float slowMotionRatio;

	BaseballPhysics* bbPhys;


	bool loaded;


	void throwPitch(int speed, vec3_t standing, vec3_t looking)	{
		entity_t* ent = createEntity();

		ent->mass = new Mass(1);

		vec3_t vel;
		VectorMA(standing, looking, speed, vel);
		VectorCopy(standing, ent->mass->pos);
		VectorCopy(vel, ent->mass->vel);

		ent->mass->instantSpeed = speed;
		ent->mass->rotationSpeed = 1500;
		CrossProduct(NORMAL_X, NORMAL_Z, ent->mass->rotationAxis);

		// 4 seconds
		ent->setTTL(4000);
		ent->collisionType = COLLISION_SPHERE;
		ent->radius = 1;
//		VectorCopy(ent->mass->pos, ent->start);	// this was used for distances... maybe on home runs? or bp?

		entityList.push_back(ent);
	}

	// call this function to load different maps
	virtual void load(string mapname)	{
		createBSP(mapname);
		getScene()->submitBSPTree(bspRoot);
		loaded = true;
	}

	// TODO make this return a leaf list when doing sphere and bounding box collisions
	// because the box or sphere may be laying in more than one partition
	bsp_node_t* findBSPLeaf(const vec3_t pos)	{
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

	void entPolyCollision(entity_t* ent, polygon_t* poly)	{
		switch( ent->collisionType )	{
		case COLLISION_NONE:	// You said it chief
			break;
		case COLLISION_BOX:		// Bounding Box vs Polygon
			// TODO perform BOX + POLY COLLISION
			break;
		case COLLISION_SPHERE:	// Sphere vs Polygon
			plane_t* plane = new plane_t;

			if( poly->hasNormals )	{
				VectorCopy(poly->normpts[0], plane->normal);
				VectorCopy(poly->points[0], plane->origin);

				vec3_t intersect;
				vec3_t rayEnd;

				VectorAdd(ent->mass->pos, ent->mass->vel, rayEnd);

				float discard;
				if( (findLinePlaneIntersect(plane, ent->mass->pos, rayEnd, intersect, &discard)) == 1 )	{
					if( isPointInPolygon(poly, intersect) )	{
						vec3_t dist;
						VectorSubtract(ent->mass->pos, intersect, dist);
						float distSquared = DotProduct(dist, dist);
						float rad = ent->radius + 0;	// no radius for polygon
						if( distSquared <= (rad*rad) )	{	// is it within striking distance?

							// TODO issue collision event so the engine can record what happened?

// WARNING: THIS COMMENTED BLOCK IS SUPER COSTLY
//							float dist = VectorDistance(ent->start, ent->mass->pos);
//							VectorCopy(ent->mass->pos, ent->start);
//
//							if( dist > 0 )
//								cout << "Distance: " << dist << "meters." << endl;
// END WARNING BLOCK
							vec3_t reflect;
							vec3_t incident;

							float len = VectorUnitVector(ent->mass->vel, incident);
							float newSpeed = len/3;

							// stop rotating when we hit objects
							// not exactly accurate but ok for now
							ent->mass->rotationSpeed = 0;

							if( newSpeed < 0.1 )	// stay still
								VectorCopy(ZERO_VECTOR, ent->mass->vel);
							else	{	// reflect damn you!
								VectorReflect(incident, poly->normpts[0], reflect);
								VectorScale(reflect, (len/3), ent->mass->vel);
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

	// TODO BROKEN!
	void entEntCollision(entity_t* ent, entity_t* otherEnt)	{
		if( !ent || !otherEnt )
			return;

		// FIXME the flag system, thanks
		// This flagging system doesn't quite work properly, am I sure I'm using it right?
		char collisionFlags;
		if( ent->collisionType == otherEnt->collisionType )
			collisionFlags = ent->collisionType | COLLISION_SAME_TYPE;
		else
			collisionFlags = ent->collisionType | otherEnt->collisionType;

		switch(collisionFlags)	{
		case COLLISION_BOX|COLLISION_SAME_TYPE:
			cout << "Box v Box Collision" << endl;
			break;

		case COLLISION_SPHERE|COLLISION_SAME_TYPE:
//				cout << "Sphere v Sphere Collision" << endl;
			break;

		case COLLISION_CYLINDER|COLLISION_SAME_TYPE:
			cout << "Cylinder v Cylinder Collision" << endl;
			break;
		case COLLISION_SPHERE|COLLISION_BOX:
			cout << "Sphere v Box Collision" << endl;
			break;

		case COLLISION_SPHERE|COLLISION_CYLINDER:
			cout << "Sphere v Cylinder Collision" << endl;
			break;

		case COLLISION_CYLINDER|COLLISION_BOX:
			cout << "Cylinder v Box Collision" << endl;
			break;
		}
	}

	void collideWithWorld(entity_t* ent, list<polygon_t*> polyList)	{
		list<polygon_t*>::iterator itr = polyList.begin();
		for(; itr != polyList.end(); itr++)	{
			entPolyCollision(ent, (*itr));
		}
	}

	void collideWithOtherEntities(entity_t* ent, list<entity_t*> entList)	{
		list<entity_t*>::iterator itr = entList.begin();
		for(; itr != entList.end(); itr++)	{
			if( ent != (*itr) )	// if its not itself
				entEntCollision(ent, (*itr));
		}
	}

	void insertEntityIntoBSP(entity_t* ent)	{
		bsp_node_t* node = findBSPLeaf(ent->mass->pos);

		collideWithWorld(ent, node->getPolygonList());

		// TODO re-enable whence fixed
//		collideWithOtherEntities(ent, node->getEntityList());

		node->addEntity(ent);
	}

	void insertEntitiesIntoBSPTree()	{
		list<entity_t*>::iterator itr = entityList.begin();

		for(;itr != entityList.end(); itr++)
			insertEntityIntoBSP((*itr));
	}


	// This is called once every time around the game loop.
	virtual void advance(long ms)	{

		if( !loaded )
			return;

		// Time work, used for Simulation work
		// dt Is The Time Interval (As Seconds) From The Previous Frame To The Current Frame.
		// dt Will Be Used To Iterate Simulation Values Such As Velocity And Position Of Masses.
		float dt = ms / 1000.0f;	// convert dt to seconds
		dt /= slowMotionRatio;										// Divide dt By slowMotionRatio And Obtain The New dt
		timeElapsed += dt;											// Iterate Elapsed Time
		float maxPossible_dt = 0.1f;								// Say That The Maximum Possible dt Is 0.1 Seconds
																	// This Is Needed So We Do Not Pass Over A Non Precise dt Value
	  	int numOfIterations = (int)(dt / maxPossible_dt) + 1;		// Calculate Number Of Iterations To Be Made At This Update Depending On maxPossible_dt And dt
		if (numOfIterations != 0)									// Avoid Division By Zero
			dt = dt / numOfIterations;								// dt Should Be Updated According To numOfIterations


		// Simulation work from here down
		if( bbPhys != NULL )	{
			for (int a = 0; a < numOfIterations; ++a)					// We Need To Iterate Simulations "numOfIterations" Times
			{
				// remove all ents from BSP Tree
				removeEntitiesFromBSPTree();
				// for number of masses do
				vector<entity_t*> removalList;

				for( list<entity_t*>::iterator itr = entityList.begin(); itr != entityList.end(); itr++)	{
					if( (*itr)->checkTTL() )	{
						removalList.push_back((*itr));
					}
					else	{
						bbPhys->operate(dt, (*itr)->mass);					// Iterate motionUnderGravitation Simulation By dt Seconds
					}
				}

				for( int x=0; x < removalList.size(); x++)	{
					bbPhys->release(removalList[x]->mass);
					entity_t* cur = removalList[x];
					cur->hasExpired = true;
					entityList.remove(cur);
					delete cur;	// TODO don't delete, clean out and put into free ent list.
				}

				insertEntitiesIntoBSPTree();
			}


		}

		getScene()->setEntityList(entityList);
	}



	// Event handlers
	// Handles keyboard input from normal text keys
	virtual void keyPressed(unsigned char key, int x, int y)	{
		Scene* curScene = getScene();

		switch(key)	{
			case '`':	// active console
				curScene->con->consoleActive = !curScene->con->consoleActive;
				break;
			case 'a':
				curScene->cam->moveCameraLeft(CAM_MOVE_RATE);
				break;
			case 'd':
				curScene->cam->moveCameraRight(CAM_MOVE_RATE);
				break;
			case 'w':
				curScene->cam->moveCameraForward(CAM_MOVE_RATE);
				break;
			case 's':
				curScene->cam->moveCameraBack(CAM_MOVE_RATE);
				break;
			case 'q':
				curScene->cam->moveCameraUp(CAM_MOVE_RATE);
				break;
			case 'e':
				curScene->cam->moveCameraDown(CAM_MOVE_RATE);
				break;
			case 'z':
				curScene->cam->rotateAboutY(-curScene->cam->yaw_rate);
				break;
			case 'x':
				curScene->cam->rotateAboutY(curScene->cam->yaw_rate);
				break;
			case 'c':
				curScene->cam->rotateAboutX(-curScene->cam->pitch_rate);
				break;
			case 'v':
				curScene->cam->rotateAboutX(curScene->cam->pitch_rate);
				break;
			case 'f':
				vec3_t r;
				VectorAdd(curScene->cam->origin, curScene->cam->normDir, r);
				throwPitch(pitchSpeed, r, curScene->cam->normDir);
				break;
			case 'g':
				vec3_t e1;
				vec3_t r1;	// pitchers mound
				r1[0] = -26.91;
				r1[1] = 1.67;
				r1[2] = 20.13;
				vec3_t d1;	// home plate
				d1[0] = -39.21;
				d1[1] = 1.67;	// 5'5"
				d1[2] = 31.86;

				VectorSubtract(d1, r1, e1);
				VectorUnitVector(e1, e1);

				throwPitch(pitchSpeed, r1, e1);
				break;
			case 'n':
				if( curScene->cam == curScene->cameras[0] )
					curScene->cam = curScene->cameras[1];
				else
					curScene->cam = curScene->cameras[0];

				break;

			case ESC_KEY:
				curScene->exit();
				break;
		}
	}

	// handles keyboard input from special keys
	virtual void specialKeyPressed(int key, int x, int y)	{

	}

	virtual void mouseEvent(int button, int state, int x, int y)	{
		Scene* cs = getScene();

		cout << "Camera Info" << endl << "Pos: ";
		VectorPrint(cs->cam->origin);
		cout << endl << "Dir: ";
		VectorPrint(cs->cam->normDir);
		cout << endl << "----------------------" << endl;
	}

	// FIXME DISABLE THIS FEATURE, THIS IS COSTLY, WTF?!
	virtual void passiveMouseEvent(int x, int y)	{
		float dx = 400-x;
		float dy = 300-y;

		if( dx == 0 && dy == 0 )
			return;

		Scene* curScene = getScene();

		dx *= 0.025;
		dy *= 0.025;

		curScene->cam->rotateAboutX(dy);	// Pitch
		curScene->cam->rotateAboutY(-dx);	// Yaw
	}


	void createBSP(string mapName)	{
		Scene* curScene = getScene();
		ObjModel* obj = loadMap(mapName);
		bspRoot = new bsp_node_t;

		generateBSPTree(bspRoot, obj->polygonList);
		curScene->nameAndCachePolygons(bspRoot);

		chdir("images/");
		curScene->sky = gluNewQuadric();
		gluQuadricTexture(curScene->sky, true);
		gluQuadricOrientation(curScene->sky, GLU_INSIDE);
		getMaterialManager()->loadBitmap(SKY_TEXTURE);
		chdir("..");
		curScene->cacheSky();
	}

	ObjModel* loadMap(string map)	{
		// TODO FIX THIS SLOPPYNESS AND THE STUFF WHEN LOADING
		// BMP FILES TOO, THIS IS JUST A TEMP HACK TO CLEAN UP
		// THE ROOT FOLDER OF THIS PROJECT.
		chdir("..");
		ObjModel* obj = new ObjModel();
		obj->loadObjFile(map);
		return obj;
	}

	void createDynamicLeafList(bsp_node_t* root, bool start)	{
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

	void removeEntitiesFromBSPTree()	{
		list<list<entity_t*> >::iterator itr;
		for(itr=entityLeafList.begin(); itr != entityLeafList.end(); itr++)
			(*itr).clear();
	}

	void addEntity(entity_t* ent)	{
		entityList.push_back(ent);
	}

	void removeEntity(entity_t* ent)	{
		entityList.remove(ent);
	}


	SpecialGame() : Game()	{
		loaded = false;
		timeElapsed = 0.0;
		slowMotionRatio = 1.0;

		bbPhys = new BaseballPhysics(GRAVITY_EARTH);
	}

	~SpecialGame()	{

	}
};
