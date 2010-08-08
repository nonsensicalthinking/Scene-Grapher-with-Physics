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
 *	Who am I and what am I trying to do?
 *	The game tells the renderer what dynamic objects are in the scene.

 *	Those dynamic objects are likely animateable models.  In the game
 *	we will refer it to as an entity.  Entities have masses that can be worked
 *	on and flags, possibly some AI. Entities also have bounding boxes (need to figure out
 *	we're going to create the bounding box automatically.

 *	Collision detection will chick dynamic objects against the objects in the
 *	bsp area they're located in.
 *
 *
 */
#include "Game.h"
#include "Scene.h"
#include "objloader.h"

#define CAM_MOVE_RATE 1

// TODO move this into the Game you extend
const vec3_t GRAVITY_EARTH = {0.0f, -9.81f, 0.0f};
// TODO Remove these 3 whence the game is up and going
//vec3_t startPos = {0.0, 0.0, 0.0};
//vec3_t startAngle = {10.0, 15.0, 0.0};



#define PITCH_RATE	1
#define YAW_RATE	1
#define ROLL_RATE	1

#define SKY_TEXTURE	"bright_clouds.bmp"

// TODO REMOVE THIS
extern int pitchSpeed;


// inherit the game class
class SpecialGame : public Game	{

	bsp_node_t* bspRoot;
	list<entity_t*> entityList;
	list<list<entity_t*> > bspDynamicEntityLeafList;

	float timeElapsed;
	float slowMotionRatio;

	MotionUnderGravitation* motionUnderGravitation;


	bool loaded;

public:

	// TODO Test ent TTL

	void throwPitch(int speed)	{
		entity_t* ent = createEntity();

		ent->mass = new Mass(1);

		vec3_t pos = {-24, 4, 19};
		vec3_t end = {-38, 4, 32};
		vec3_t vel;
		VectorSubtract(pos, end, vel);
		VectorUnitVector(vel, vel);
		VectorScale(vel, speed, vel);
		VectorCopy(pos, ent->mass->pos);
		VectorNegate(vel, vel);
		VectorCopy(vel, ent->mass->vel);

		// 5 seconds
		ent->setTTL(2000);

		entityList.push_back(ent);

		cout << "And the pitch..." << endl;
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

	// This is called once every time around the game loop.
	virtual void advance(long ms)	{

		if( !loaded )
			return;

		// Time work, used for Simulation work
		// dt Is The Time Interval (As Seconds) From The Previous Frame To The Current Frame.
		// dt Will Be Used To Iterate Simulation Values Such As Velocity And Position Of Masses.
		float dt = ms / 1000.0f;							// Let's Convert Milliseconds To Seconds
		dt /= slowMotionRatio;										// Divide dt By slowMotionRatio And Obtain The New dt
		timeElapsed += dt;											// Iterate Elapsed Time
		float maxPossible_dt = 0.1f;								// Say That The Maximum Possible dt Is 0.1 Seconds
																	// This Is Needed So We Do Not Pass Over A Non Precise dt Value
	  	int numOfIterations = (int)(dt / maxPossible_dt) + 1;		// Calculate Number Of Iterations To Be Made At This Update Depending On maxPossible_dt And dt
		if (numOfIterations != 0)									// Avoid Division By Zero
			dt = dt / numOfIterations;								// dt Should Be Updated According To numOfIterations


		// Simulation work from here down
		if( motionUnderGravitation != NULL )	{

			for (int a = 0; a < numOfIterations; ++a)					// We Need To Iterate Simulations "numOfIterations" Times
			{
				// for number of masses do
				vector<entity_t*> removalList;

				for( list<entity_t*>::iterator itr = entityList.begin(); itr != entityList.end(); itr++)	{
					if( (*itr)->checkTTL() )	{
						removalList.push_back((*itr));
					}
					else	{
						motionUnderGravitation->operate(dt, (*itr)->mass);					// Iterate motionUnderGravitation Simulation By dt Seconds
					}
				}

				for( int x=0; x < removalList.size(); x++)	{
					motionUnderGravitation->release(removalList[x]->mass);
					entity_t* cur = removalList[x];
					cur->hasExpired = true;
					entityList.remove(cur);
					delete cur;	// TODO don't delete, clean out and put into free ent list.
					cout << "Entity removed from scene! (" << entityList.size() << ")."<< endl;
				}
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
			case 'a':	// omg our first control over the scene!
				curScene->cam->moveCameraLeft(CAM_MOVE_RATE);
				break;
			case 'd':	// omg our first control over the scene!
				curScene->cam->moveCameraRight(CAM_MOVE_RATE);
				break;
			case 'w':	// omg our first control over the scene!
				curScene->cam->moveCameraForward(CAM_MOVE_RATE);
				break;
			case 's':	// omg our first control over the scene!
				curScene->cam->moveCameraBack(CAM_MOVE_RATE);
				break;
			case 'q':	// omg our first control over the scene!
				curScene->cam->moveCameraUp(CAM_MOVE_RATE);
				break;
			case 'e':	// omg our first control over the scene!
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
				throwPitch(pitchSpeed);
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
			bspDynamicEntityLeafList.clear();

		if( root->isLeaf() )	{
			bspDynamicEntityLeafList.push_back(root->getDynamicObjectList());
		}
		else	{
			createDynamicLeafList(root->front, false);
			createDynamicLeafList(root->back, false);
		}
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

		motionUnderGravitation = new MotionUnderGravitation(GRAVITY_EARTH);
	}

	~SpecialGame()	{

	}
};
