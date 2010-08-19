/**************************************************************************

  File: physics.h
	Portions of the code in this file were derived from the works of:
	Erkin Tunca for http://nehe.gamedev.net/
 	More specifically the introduction to physical simulations lesson.
	Thanks for the lesson.

**************************************************************************/


#ifndef PHYSICS_H_
#define PHYSICS_H_

#include <math.h>
#include "shared.h"

extern int sceneAdvRate;

// Returns angular speed in rad/ms
inline float RPMtoAngularSpeed(float rpm)	{
	return (TWO_PI_DIV_BY_60 * rpm) * (ONE_MILLISECOND * sceneAdvRate);
}

// this only does drag coefficient for baseballs
// FIXME make this an index table instead
inline float getDragCoeff(int speed)	{
	float drag = 0.0;

	if( speed > 100 )
		drag = 0.22;
	else if( speed > 90 )
		drag = 0.275;
	else if( speed > 80 )
		drag = 0.3;
	else if( speed > 70 )
		drag = 0.4;
	else if( speed > 60 )
		drag = 0.55;
	else if( speed < 60 )
		drag = 0.6;
	else
		drag = 0.6;

	return drag;
}


class Mass	{
public:
	float m;					// The mass value
	float rotationSpeed;		// In RPM?
	float instantSpeed;			// instantaneousSpeed

	vec3_t prevPos;				// previous position
	vec3_t pos;					// Position in space
	vec3_t vel;					// Velocity
	vec3_t force;				// Force applied on this mass at an instance
	vec3_t rotationAxis;		// Axis of rotation


	Mass(float m)	{
		this->m = m;
	}

	void applyForce(vec3_t force)	{
		VectorAdd(this->force, force, this->force);
	}

	void init()	{
		force[0] = 0;
		force[1] = 0;
		force[2] = 0;
	}

	void simulate(float dt)	{
		vec3_t velocityDelta;

		VectorDivide(force, m, velocityDelta);
		VectorMA(vel, velocityDelta, dt, vel);

		VectorCopy(pos, prevPos);		// save old position

		VectorMA(pos, vel, dt, pos);	// get new one!
	}

};

// Abstract class Simulation
class Simulation	{
public:
	
	Simulation()	{
	}

	virtual void release(Mass* mass)	{
		delete(mass);
		mass = NULL;
	}

	virtual void init(Mass* mass)	{
		mass->init();
	}

	virtual void solve(Mass* mass)	{
	}

	virtual void simulate(float dt, Mass* mass)	{
		mass->simulate(dt);
	}

	virtual void operate(float dt, Mass* mass)	{
		init(mass);
		solve(mass);
		simulate(dt, mass);
	}

};


// Physics of a baseball in flight
class BaseballPhysics : public Simulation	{
public:
	vec3_t gravitation;													//the gravitational acceleration

	BaseballPhysics(const vec3_t gravitation) : Simulation()	{																		//Vector3D gravitation, is the gravitational acceleration
		VectorCopy(gravitation, this->gravitation);
	}

	virtual void solve(Mass* mass)	{
		// Do gravity
		vec3_t gravity;
		VectorScale(gravitation, mass->m, gravity);
		mass->applyForce(gravity);
		float dragCoeff = getDragCoeff(mass->instantSpeed);

		// Do Aerodynamic Drag
		vec3_t dragForce;
		VectorScale(mass->vel, dragCoeff, dragForce);
		VectorNegate(dragForce, dragForce);
		mass->applyForce(dragForce);

		// Do Magnus Force
		if( mass->rotationSpeed != 0 )	{
			vec3_t result;
			mass->instantSpeed = VectorLength(mass->vel);

// its a beautiful thing!
//			cout << "Speed: " << mass->instantSpeed << endl;

			CrossProduct(NORMAL_Y, mass->vel, result);
			VectorScale(result, dragCoeff, result);

			float rotSpeedRad = RPMtoAngularSpeed(mass->rotationSpeed);
			VectorScale(result, rotSpeedRad, result);

			mass->applyForce(result);
		}

	}
	
};



//  class ConstantVelocity is derived from class Simulation
//  It creates 1 mass with mass value 1 kg and sets its velocity to (1.0f, 0.0f, 0.0f)
//  so that the mass moves in the x direction with 1 m/s velocity.
/*
class ConstantVelocity : public Simulation	{
public:
	ConstantVelocity() : Simulation(1, 1.0f)	{
		vec3_t startPosition = {0.0f, 0.0f, 0.0f};
		vec3_t startVelocity = {1.0f, 0.0f, 0.0f};

		VectorCopy(startPosition, masses[0]->pos);
		VectorCopy(startVelocity, masses[0]->vel);
	}

};
*/



//	class MassConnectedWithSpring is derived from class Simulation
//	It creates 1 mass with mass value 1 kg and binds the mass to an arbitrary constant point with a spring.
//	This point is referred as the connectionPos and the spring has a springConstant value to represent its
//	stiffness.
/*
class MassConnectedWithSpring : public Simulation	{
public:
	float springConstant;
	vec3_t connectionPos;

	MassConnectedWithSpring(const float springConstant) : Simulation(1, 1.0f)	{
		this->springConstant = springConstant;

		vec3_t attachmentPosition = {0.0f, -5.0f, 0.0f};

		VectorCopy(attachmentPosition, connectionPos);

		vec3_t pullBackDistance = {10.0f, 0.0f, 0.0f};
		vec3_t startVelocity = {0.0, 0.0, 0.0};

		VectorAdd(connectionPos, pullBackDistance, masses[0]->pos);
		VectorCopy(startVelocity, masses[0]->vel);

	}

	virtual void solve()	{
		vec3_t springVector;
		vec3_t springForce;

		for (int a = 0; a < numOfMasses; ++a)	{
			VectorSubtract(masses[a]->pos, connectionPos, springVector);
			VectorNegate(springVector, springVector);
			VectorScale(springVector, springConstant, springForce);
			masses[a]->applyForce(springForce);
		}
	}
};
*/

#endif	// ENDIF PHYSICS_H_


