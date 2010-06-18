/**************************************************************************

  File: Physics.h
	Portions of the code in this file was derived from the works of:
	Erkin Tunca for http://nehe.gamedev.net

	Thanks for the lesson.

**************************************************************************/


#ifndef PHYSICS_H_
#define PHYSICS_H_

#include <math.h>
#include "shared.h"

// class Mass			---> An object to represent a mass
class Mass
{
public:
	float m;									// The mass value
	vec3_t pos;								// Position in space
	vec3_t vel;								// Velocity
	vec3_t force;								// Force applied on this mass at an instance

	Mass(float m)								// Constructor
	{
		this->m = m;
	}

	/*
	  void applyForce(Vector3D force) method is used to add external force to the mass. 
	  At an instance in time, several sources of force might affect the mass. The vector sum 
	  of these forces make up the net force applied to the mass at the instance.
	*/
	void applyForce(vec3_t force)
	{
// start old code
		//this->force += force;					// The external force is added to the force of the mass
// end old code
		VectorAdd(this->force, force, this->force);
	}

	/*
	  void init() method sets the force values to zero
	*/
	void init()
	{
		force[0] = 0;
		force[1] = 0;
		force[2] = 0;
	}

	/*
	  void simulate(float dt) method calculates the new velocity and new position of 
	  the mass according to change in time (dt). Here, a simulation method called
	  "The Euler Method" is used. The Euler Method is not always accurate, but it is 
	  simple. It is suitable for most of physical simulations that we know in common 
	  computer and video games.
	*/
	void simulate(float dt)
	{
// start old code
//		vel += (force / m) * dt;				// Change in velocity is added to the velocity.
												// The change is proportinal with the acceleration (force / m) and change in time

//		pos += vel * dt;						// Change in position is added to the position.
												// Change in position is velocity times the change in time
// end old code

		vec3_t velocityDelta;

		VectorDivide(force, m, velocityDelta);
		VectorMA(vel, velocityDelta, dt, vel);

		VectorMA(pos, vel, dt, pos);
	}

};

// class Simulation		---> A container object for simulating masses
class Simulation
{
public:
	int numOfMasses;								// number of masses in this container
	Mass** masses;									// masses are held by pointer to pointer. (Here Mass** represents a 1 dimensional array)
	
	Simulation(int numOfMasses, float m)			// Constructor creates some masses with mass values m
	{
		this->numOfMasses = numOfMasses;
		
		masses = new Mass*[numOfMasses];			// Create an array of pointers

		for (int a = 0; a < numOfMasses; ++a)		// We will step to every pointer in the array
			masses[a] = new Mass(m);				// Create a Mass as a pointer and put it in the array
	}

	virtual void release()							// delete the masses created
	{
		for (int a = 0; a < numOfMasses; ++a)		// we will delete all of them
		{
			delete(masses[a]);
			masses[a] = NULL;
		}
			
		delete(masses);
		masses = NULL;
	}

	Mass* getMass(int index)
	{
		if (index < 0 || index >= numOfMasses)		// if the index is not in the array
			return NULL;							// then return NULL

		return masses[index];						// get the mass at the index
	}

	virtual void init()								// this method will call the init() method of every mass
	{
		for (int a = 0; a < numOfMasses; ++a)		// We will init() every mass
			masses[a]->init();						// call init() method of the mass
	}

	virtual void solve()							// no implementation because no forces are wanted in this basic container
	{
													// in advanced containers, this method will be overrided and some forces will act on masses
	}

	virtual void simulate(float dt)					// Iterate the masses by the change in time
	{
		for (int a = 0; a < numOfMasses; ++a)		// We will iterate every mass
			masses[a]->simulate(dt);				// Iterate the mass and obtain new position and new velocity
	}

	virtual void operate(float dt)					// The complete procedure of simulation
	{
		init();										// Step 1: reset forces to zero
		solve();									// Step 2: apply forces
		simulate(dt);								// Step 3: iterate the masses by the change in time
	}

};

/*
  class ConstantVelocity is derived from class Simulation
  It creates 1 mass with mass value 1 kg and sets its velocity to (1.0f, 0.0f, 0.0f)
  so that the mass moves in the x direction with 1 m/s velocity.
*/
class ConstantVelocity : public Simulation
{
public:
	ConstantVelocity() : Simulation(1, 1.0f)				//Constructor firstly constructs its super class with 1 mass and 1 kg
	{
		vec3_t startPosition = {0.0f, 0.0f, 0.0f};
		vec3_t startVelocity = {1.0f, 0.0f, 0.0f};

		VectorCopy(startPosition, masses[0]->pos);		//a mass was created and we set its position to the origin
		VectorCopy(startVelocity, masses[0]->vel);		//we set the mass's velocity to (1.0f, 0.0f, 0.0f)
	}

};

/*
  class MotionUnderGravitation is derived from class Simulation
  It creates 1 mass with mass value 1 kg and sets its velocity to (10.0f, 15.0f, 0.0f) and its position to 
  (-10.0f, 0.0f, 0.0f). The purpose of this application is to apply a gravitational force to the mass and 
  observe the path it follows. The above velocity and position provides a fine projectile path with a 
  9.81 m/s/s downward gravitational acceleration. 9.81 m/s/s is a very close value to the gravitational 
  acceleration we experience on the earth.
*/
class MotionUnderGravitation : public Simulation
{
public:
	vec3_t gravitation;													//the gravitational acceleration

	MotionUnderGravitation(const vec3_t gravitation, const vec3_t startingPosition, const vec3_t startingVelocity) : Simulation(1, 1.0f)		//Constructor firstly constructs its super class with 1 mass and 1 kg
	{																		//Vector3D gravitation, is the gravitational acceleration
		VectorCopy(gravitation, this->gravitation);									//set this class's gravitation
		VectorCopy( startingPosition, masses[0]->pos);						//set the position of the mass
		VectorCopy( startingVelocity, masses[0]->vel);						//set the velocity of the mass
	}

	virtual void solve()													//gravitational force will be applied therefore we need a "solve" method.
	{
// start old code
//		for (int a = 0; a < numOfMasses; ++a)								//we will apply force to all masses (actually we have 1 mass, but we can extend it in the future)
//			masses[a]->applyForce(gravitation * masses[a]->m);				//gravitational force is as F = m * g. (mass times the gravitational acceleration)
// end old code

		for (int a = 0; a < numOfMasses; ++a)	{
			vec3_t force;

			VectorScale(gravitation, masses[a]->m, force);

			masses[a]->applyForce(force);
		}

	}
	
};

/*
  class MassConnectedWithSpring is derived from class Simulation
  It creates 1 mass with mass value 1 kg and binds the mass to an arbitrary constant point with a spring. 
  This point is refered as the connectionPos and the spring has a springConstant value to represent its 
  stiffness.
*/
class MassConnectedWithSpring : public Simulation
{
public:
	float springConstant;													//more the springConstant, stiffer the spring force
	vec3_t connectionPos;													//the arbitrary constant point that the mass is connected

	MassConnectedWithSpring(const float springConstant) : Simulation(1, 1.0f)		//Constructor firstly constructs its super class with 1 mass and 1 kg
	{
		this->springConstant = springConstant;								//set the springConstant

		vec3_t attachmentPosition = {0.0f, -5.0f, 0.0f};

		VectorCopy(attachmentPosition, connectionPos);						//set the connectionPos

// start old code
		//		masses[0]->pos = connectionPos + Vector3D(10.0f, 0.0f, 0.0f);		//set the position of the mass 10 meters to the right side of the connectionPos
		//		masses[0]->vel = Vector3D(0.0f, 0.0f, 0.0f);						//set the velocity of the mass to zero
// end old code

		vec3_t pullBackDistance = {10.0f, 0.0f, 0.0f};
		vec3_t startVelocity = {0.0, 0.0, 0.0};

		VectorAdd(connectionPos, pullBackDistance, masses[0]->pos);
		VectorCopy(startVelocity, masses[0]->vel);

	}

	virtual void solve()													//the spring force will be applied
	{
		vec3_t springVector;
		vec3_t springForce;

		for (int a = 0; a < numOfMasses; ++a)								//we will apply force to all masses (actually we have 1 mass, but we can extend it in the future)
		{
// start old code
//			Vector3D springVector = masses[a]->pos - connectionPos;			//find a vector from the position of the mass to the connectionPos
//			masses[a]->applyForce(-springVector * springConstant);			//apply the force according to the famous spring force formulation
// end old code

			VectorSubtract(masses[a]->pos, connectionPos, springVector);
			VectorNegate(springVector, springVector);
			VectorScale(springVector, springConstant, springForce);
			masses[a]->applyForce(springForce);
		}
	}
	
};

#endif	// ENDIF PHYSICS_H_


