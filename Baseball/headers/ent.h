/*
 * ent.h
 *
 *  Created on: Aug 3, 2010
 *      Author: brof
 */

#include "physics.h"

#ifndef ENT_H_
#define ENT_H_

#define COLLISION_NONE		0x01	// 1
#define COLLISION_BOX		0x02	// 2
#define COLLISION_SPHERE	0x04	// 4
#define COLLISION_CYLINDER	0x08	// 8
#define COLLISION_SAME_TYPE	0x10	// 16

extern long Sys_Milliseconds(void);


typedef struct entity_s	{
	unsigned int entID;
	int expired;
	bool hasExpired;

	string md2name;

	Mass* mass;	// Mass' current state

	// Collision Stuffs
	char collisionType;	// stores bit flag for type of collision
	float radius;	// for sphere collision
//	cylinder_t* cylinder;	// constraints of cylinder
//	bbox_t*	boundingBox; 	// constraints of bounding box
	// end collision stuffs

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
	ent->md2name = "";
	ent->collisionType = COLLISION_NONE;
}


#endif /* ENT_H_ */
