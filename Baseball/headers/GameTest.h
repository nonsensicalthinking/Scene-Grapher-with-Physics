/*
 * GameTest.h
 *
 *  Created on: Jul 26, 2010
 *      Author: brof
 *
 *
 *  TODO This particular todo may belong in the Game class...
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
 *	TODO Insert Entities into the bsp tree as soon as we operate on them.
 *
 */
#include "Game.h"
#include "Scene.h"
#include "objloader.h"
#include "shared.h"
#include "ent.h"
#include <queue>
#define CAM_MOVE_RATE 	1


#define SKY_TEXTURE		"partly_cloudy.bmp"

#define IMAGE_DIRECTORY	"images/"


// TODO REMOVE THIS
extern int pitchSpeed;

const vec3_t GRAVITY_EARTH = {0.0f, -9.81f, 0.0f};


// inherit the game class
class SpecialGame : public Game	{
	bool fullScreen;

public:

	bsp_node_t* bspRoot;
	list<entity_t*> entityList;
	list<list<entity_t*> > entityLeafList;
	queue<entity_t*> unusedEnts;


	float timeElapsed;
	float slowMotionRatio;

	BaseballPhysics* bbPhys;


	bool loaded;

	// X PINCH POINT X
	// not thread safe
	void recycleEntity(entity_t* ent)	{
		cleanEntity(ent);
		unusedEnts.push(ent);
	}

	// you're responsible for getting one...
	// this reference may or may not come back NULL
	entity_t* getFreshEntity()	{
		entity_t* ent = unusedEnts.front();
		unusedEnts.pop();
		return ent;
	}

	void throwPitch(int speed, vec3_t standing, vec3_t looking)	{
		entity_t* ent = getFreshEntity();

		ent->mass = new Mass(1);

		vec3_t vel;
		VectorMA(standing, looking, speed, vel);
		VectorCopy(standing, ent->mass->pos);
		VectorCopy(vel, ent->mass->vel);

		ent->mass->moveType = MOVE_TYPE_BASEBALL;
		ent->mass->instantSpeed = speed;
		ent->mass->rotationSpeed = 1500;
		CrossProduct(NORMAL_X, NORMAL_Z, ent->mass->rotationAxis);

		// 4 seconds
		ent->parishable = true;
		ent->setTTL(4000);
		ent->collisionType = COLLISION_SPHERE;
		ent->radius = 1;
//		VectorCopy(ent->mass->pos, ent->start);	// this was used for distances... maybe on home runs? or bp?

		entityList.push_back(ent);
	}

	void placePlayer(vec3_t pos, vec3_t facing)	{
		entity_t* ent = getFreshEntity();

		// movement info
		ent->mass = new Mass(1);

//		vec3_t vel;
//		VectorMA(standing, looking, speed, vel);
		VectorCopy(pos, ent->mass->pos);
//		VectorCopy(vel, ent->mass->vel);

		ent->mass->moveType = MOVE_TYPE_AT_REST;
//		ent->mass->instantSpeed = speed;
//		ent->mass->rotationSpeed = 1500;
//		CrossProduct(NORMAL_X, NORMAL_Z, ent->mass->rotationAxis);


		// Lifetime info
		ent->parishable = false;
//		ent->setTTL(4000);		// 4 seconds


		// Collision info
		ent->collisionType = COLLISION_SPHERE;
		ent->radius = 1;

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
							vec3_t reflect;
							vec3_t incident;

							float len = VectorUnitVector(ent->mass->vel, incident);
							float newSpeed = len * 0.33;	// Shrink by 1/3~

							// stop rotating when we hit objects
							// not exactly accurate but ok for now
							ent->mass->rotationSpeed = 0;

							if( newSpeed < 0.1 )	// stay still
								ent->mass->moveType = MOVE_TYPE_AT_REST;
							else	{	// reflect damn you!
								VectorReflect(incident, poly->normpts[0], reflect);
								VectorScale(reflect, newSpeed, ent->mass->vel);
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

	void entEntCollision(entity_t* ent, entity_t* otherEnt)	{
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
		    	VectorMA(ball->mass->pos, ball->mass->vel, lamda, newposition);

//		    	TVector HB=newposition-cylinder._Position;

//		    	pNormal=HB - cylinder._Axis*(HB.dot(cylinder._Axis));
//				pNormal.unit();

			}
			/*
			TVector RC;
			double d;
			double t,s;
			TVector n,D,O;
			double ln;
			double in,out;


			TVector::subtract(position,cylinder._Position,RC);
			TVector::cross(direction,cylinder._Axis,n);

		    ln=n.mag();

			if ( (ln<ZERO)&&(ln>-ZERO) ) return 0;

			n.unit();

			d= fabs( RC.dot(n) );

		    if (d<=cylinder._Radius)
			{
				TVector::cross(RC,cylinder._Axis,O);
				t= - O.dot(n)/ln;
				TVector::cross(n,cylinder._Axis,O);
				O.unit();
				s= fabs( sqrt(cylinder._Radius*cylinder._Radius - d*d) / direction.dot(O) );

				in=t-s;
				out=t+s;

				if (in<-ZERO){
					if (out<-ZERO) return 0;
					else lamda=out;
				}
				else
		        if (out<-ZERO) {
					      lamda=in;
				}
				else
				if (in<out) lamda=in;
				else lamda=out;

		    	newposition=position+direction*lamda;
				TVector HB=newposition-cylinder._Position;
				pNormal=HB - cylinder._Axis*(HB.dot(cylinder._Axis));
				pNormal.unit();

				return 1;
			}
		    */











			break;

		case COLLISION_CYLINDER|COLLISION_BOX:
			cout << "Implement Cylinder v Box Collision" << endl;
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

		collideWithOtherEntities(ent, node->getEntityList());

		node->addEntity(ent);
	}


	// FIXME
	// I do believe this is why we should crawl entities along surfaces...
	// this is costly to do ... every frame!
	void insertEntitiesIntoBSPTree()	{
		list<entity_t*>::iterator itr = entityList.begin();

		for(;itr != entityList.end(); itr++)
			insertEntityIntoBSP((*itr));
	}


	// This is called once every time around the game loop.
	virtual void advance(float dSec)	{

		if( !loaded )
			return;

		// Time work, used for Simulation work
		// dt Is The Time Interval (As Seconds) From The Previous Frame To The Current Frame.
		// dt Will Be Used To Iterate Simulation Values Such As Velocity And Position Of Masses.

		float maxPossible_dt = 0.1f;	// Cap time step to 0.1 sec


// commenting this out shaved off 1 fps
// before you un-comment, use multiplication instead!
//		dt /= slowMotionRatio;		// Divide dt By slowMotionRatio And Obtain The New dt

		timeElapsed += dSec;											// Iterate Elapsed Time
																	// This Is Needed So We Do Not Pass Over A Non Precise dt Value

		int numOfIterations = (int)(dSec / maxPossible_dt) + 1;		// Calculate Number Of Iterations To Be Made At This Update Depending On maxPossible_dt And dt


	  	if (numOfIterations != 0)						// Avoid Division By Zero
			dSec = dSec / numOfIterations;					// dt Should Be Updated According To numOfIterations


		// Simulation work from here down
		if( bbPhys != NULL )	{
			for (int a = 0; a < numOfIterations; ++a)	// We Need To Iterate Simulations "numOfIterations" Times
			{
				// remove all ents from BSP Tree
				removeEntitiesFromBSPTree();
				// for number of masses do
				vector<entity_t*> removalList;

				for( list<entity_t*>::iterator itr = entityList.begin(); itr != entityList.end(); itr++)	{
					if( (*itr)->parishable && (*itr)->checkTTL() )	{
						removalList.push_back((*itr));
					}
					else	{
						switch((*itr)->mass->moveType)	{
							case MOVE_TYPE_AT_REST:
								break;
							case MOVE_TYPE_BASEBALL:
								bbPhys->operate(dSec, (*itr)->mass);	// iterate the entity by dt seconds
								break;
						}

						// TODO Insert entity once its been operated instead of calling another
						// function to call a loop to do it.
					}
				}

				for( int x=0; x < removalList.size(); x++)	{
					bbPhys->release(removalList[x]->mass);
					entity_t* cur = removalList[x];
					cur->hasExpired = true;
					entityList.remove(cur);
					recycleEntity(cur);					// make entity available for use again.
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
			case '1':
				placePlayer(curScene->cam->origin, curScene->cam->normDir);
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
			case 'm':
				curScene->fullScreen(fullScreen);
				fullScreen = !fullScreen;
				break;
			case 'p':
				entity_t* cyl;
				cyl = createEntity();
				cyl->cylinder = new cylinder_t;
				vec3_t plate;	// home plate
				plate[0] = -39.21;
				plate[1] = 1.67;	// 5'5"
				plate[2] = 31.86;
				cyl->mass = new Mass(1);
				VectorCopy(plate, cyl->mass->pos);
				cyl->mass->moveType = MOVE_TYPE_AT_REST;
				cyl->cylinder->centerAxis[0] = 1.0;
				cyl->cylinder->centerAxis[1] = 0.0;
				cyl->cylinder->centerAxis[2] = 0.0;
				cyl->cylinder->radius = 0.028575;	// bat radius in meters
//				cyl->cylinder->normal;	// later
				cyl->parishable = false;
				cyl->collisionType = COLLISION_CYLINDER;
				entityList.push_back(cyl);
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
	// (probably polling the mouse, passive is a misnomer)
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

		// TODO INCLUDE THIS WITH THE MAP!
		// THIS IS JUST FOR LOADING THE SKY
		chdir("images/");

		curScene->sky = gluNewQuadric();
		gluQuadricTexture(curScene->sky, true);
		gluQuadricOrientation(curScene->sky, GLU_INSIDE);
		getMaterialManager()->loadBitmap(SKY_TEXTURE);

		chdir("..");
		curScene->cacheSky();
		// END LOADING THE SKY
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

		// fill ent pool
		for(int x=0; x < 100; x++)
			unusedEnts.push(createEntity());


		bbPhys = new BaseballPhysics(GRAVITY_EARTH);
	}

	~SpecialGame()	{

	}
};
