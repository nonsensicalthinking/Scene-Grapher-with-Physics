/*
 * ent.h
 *
 *  Created on: Aug 3, 2010
 *      Author: brof
 */

#include "physics.h"

#ifndef ENT_H_
#define ENT_H_

#define COLLISION_NONE		0
#define COLLISION_BOX		1
#define COLLISION_SPHERE	2
#define COLLISION_CYLINDER	3


extern int Sys_Milliseconds(void);


typedef struct entity_s	{
	unsigned int entID;
	int expired;
	bool hasExpired;

	Mass* mass;	// Mass' current state
	Mass* preMass;	// Mass' previous state

	string md2name;

	int collisionType;
	// Bounding box def
	// Sphere def
	// Cylinder def

	// flags for gameplay

	// Returns TRUE if time is expired
	// FALSE otherwise
	bool checkTTL()	{
		if( Sys_Milliseconds() >= expired )
			return true;

		return false;
	}

	void setTTL(int ms)	{
		expired = Sys_Milliseconds() + ms;
	}

}entity_t;


inline entity_t* createEntity()	{
	static unsigned int entCount = 0;
	entity_t* ent = new entity_t;
	ent->entID = entCount++;
	ent->hasExpired = false;
	ent->expired = 0;
	ent->mass = NULL;
	ent->preMass = NULL;
	ent->md2name = "";
	ent->collisionType = COLLISION_NONE;
//	ent->curPos = ent->curMass->pos;
}


#endif /* ENT_H_ */
