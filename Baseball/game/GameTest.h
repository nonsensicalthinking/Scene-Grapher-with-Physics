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
#include <queue>
#include "Game.h"
#include "Scene.h"
#include "objloader.h"
#include "shared.h"
#include "ent.h"

#ifdef _WIN32
#include <direct.h>
#endif// WINDOWS

#define CAM_MOVE_RATE 	1

#ifndef GAME_TEST_H_
#define GAME_TEST_H_

#define SKY_TEXTURE		"partly_cloudy.bmp"

#define IMAGE_DIRECTORY	"images/"


// TODO REMOVE THIS
extern int pitchSpeed;
extern float maxPossible_dt;

const vec3_t GRAVITY_EARTH = {0.0f, -9.81f, 0.0f};

// inherit the game class
class SpecialGame : public Game	{
	bool fullScreen;

public:
	BaseballPhysics* bbPhys;

	// This is called once every time around the game loop.
	virtual void advance(float dSec)	{

		if( !loaded )
			return;

		// Time work, used for Simulation work
		// dt Is The Time Interval (As Seconds) From The Previous Frame To The Current Frame.
		// dt Will Be Used To Iterate Simulation Values Such As Velocity And Position Of Masses.


// commenting this out shaved off 1 fps
// before you un-comment, use multiplication instead!
//		dt /= slowMotionRatio;		// Divide dt By slowMotionRatio And Obtain The New dt

		timeElapsed += dSec;											// Iterate Elapsed Time
																	// This Is Needed So We Do Not Pass Over A Non Precise dt Value

		// test comment, this had +1 added to it...
		int numOfIterations = (int)(dSec / maxPossible_dt);		// Calculate Number Of Iterations To Be Made At This Update Depending On maxPossible_dt And dt

//		int numOfIterations = (dSec * 125);

//		cout << "** numOfIterations: " << numOfIterations << "\n";
//		cout << "** New numOfIterations: " << testNumOfIterations << "\n";

	  	if (numOfIterations != 0)						// Avoid Division By Zero
			dSec = dSec / numOfIterations;					// dt Should Be Updated According To numOfIterations


		// Simulation work from here down
		if( bbPhys != NULL )	{

			for (int a = 0; a < numOfIterations; ++a)	// We Need To Iterate Simulations "numOfIterations" Times
			{
				// we don't need to remove all ents, when re-adding it will re-classify (I think, investigate this)
	//			removeEntitiesFromBSPTree();

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

		// FIXME
		// TODO Get entities into the scene some how....
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

		ent->md2name = "smallguy.md2";

		ent->model = getScene()->modelManager->loadMD2Model(ent->md2name);
		ent->model->setAnimation("run");
		VectorCopy(pos, ent->mass->pos);
		VectorCopy(facing, ent->facing);

		ent->mass->moveType = MOVE_TYPE_AT_REST;

		// Lifetime info
		ent->parishable = false;

		// Collision info
		ent->collisionType = COLLISION_SPHERE;
		ent->radius = 1;

		entityList.push_back(ent);
	}



	void insertEntitiesIntoBSPTree()	{
		list<entity_t*>::iterator itr = entityList.begin();

		for(;itr != entityList.end(); itr++)
			insertEntityIntoBSP((*itr));
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
				break;
			case 'f':
				throwPitch(120, curScene->cam->origin, curScene->cam->normDir);
				break;
			case 'g':
				break;
			case 'n':
				if( curScene->cam == curScene->cameras[0] )
					curScene->cam = curScene->cameras[1];
				else
					curScene->cam = curScene->cameras[0];
				break;
			case 'm':
				break;
			case 'p':	// create bat in space to hit
				break;
			case 'o':	// place player
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
		cout << endl << "Angles: ";
		VectorPrint(cs->cam->angles);
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

		dx *= 0.0025;
		dy *= 0.0025;

		curScene->cam->rotateAboutX(dy);	// Pitch
		curScene->cam->rotateAboutY(-dx);	// Yaw
	}




	void loadSky()	{
		Scene* curScene = getScene();

		// TODO INCLUDE THIS WITH THE MAP!
		// THIS IS JUST FOR LOADING THE SKY
		string imagePath = "images/";

		chdir(imagePath.c_str());

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

	void createBSP(string mapName)	{
		Scene* curScene = getScene();
		ObjModel* obj = loadMap(mapName);
		bspRoot = new bsp_node_t;
		bspRoot->root = true;

		// FIXME this diameter is static (60), but should be set to width/2 of map...
		generateBSPTree(bspRoot, obj->polygonList, 100);
		curScene->nameAndCachePolygons(bspRoot);

		delete obj;
	}

	// call this function to load different maps
	virtual void load(string mapname)	{
		createBSP(mapname);
		loaded = true;
	}

	SpecialGame() : Game()	{
		bspRoot = NULL;
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

#endif

