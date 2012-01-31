/*
 * ent.h
 *
 *  Created on: Aug 3, 2010
 *      Author: brof
 */

#include "physics.h"
#include "md2model.h"

#ifndef ENT_H_
#define ENT_H_

#define COLLISION_NONE		0x01	// 1
#define COLLISION_BOX		0x02	// 2
#define COLLISION_SPHERE	0x04	// 4
#define COLLISION_CYLINDER	0x08	// 8
#define COLLISION_SAME_TYPE	0x10	// 16

extern long timeStamp;	// our current time reference in milliseconds

typedef struct cylinder_s	{
	vec3_t centerAxis;
	vec3_t normal;	// will be used for telling which direction the bat is moving
	float radius;

}cylinder_t;

typedef struct entity_s	{
	unsigned int entID;
	bool parishable;
	int expired;
	bool hasExpired;

	vec3_t facing;

	string md2name;
	MD2Model* model;

	Mass* mass;	// Mass' current state

	// Collision Stuffs
	char collisionType;	// stores bit flag for type of collision
	float radius;	// for sphere collision
	cylinder_t* cylinder;	// constraints of cylinder
//	bbox_t*	boundingBox; 	// constraints of bounding box
	// end collision stuffs

	// flags for gameplay

	// Returns TRUE if time is expired
	// FALSE otherwise
	bool checkTTL()	{
		if( timeStamp >= expired )
			return true;

		return false;
	}

	void setTTL(int ms)	{
		expired = timeStamp + ms;
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
	ent->model = NULL;
	ent->collisionType = COLLISION_NONE;

	ent->facing[0] = 1.0;
	ent->facing[1] = 0;
	ent->facing[2] = 0;

	return ent;
}

inline void cleanEntity(entity_t* ent)	{
	ent->hasExpired = false;
	ent->expired = 0;
//	if( ent->mass )
//		delete ent->mass;
	ent->mass = NULL;
	ent->md2name = "";
//	if( ent->model )
//		delete ent->model;
	ent->model = NULL;
	ent->collisionType = COLLISION_NONE;
	ent->radius = 0;
}


#endif /* ENT_H_ */
