/*
 * RayHitStructs.h
 */

#ifndef CORE_RAYHITSTRUCTS_H_
#define CORE_RAYHITSTRUCTS_H_

#include "math/geometry.h"

namespace rt{

/*
 * Ray structure definition
 */
enum RayType {PRIMARY, SECONDARY, SHADOW, REFRACT, BADRAY};

struct Ray{
	Ray(){};

	Ray(Vec3f origin, Vec3f direction, RayType raytype): raytype(raytype), origin(origin), direction(direction) {};

	RayType raytype = BADRAY;

	Vec3f origin;

	Vec3f direction;

	//----------Ray variables to be filled------
};


struct Hit{
	Hit(){};
	
	bool isHit = false;
	Vec3f point; //point where ray hits a shape
	//----------Hit variables to be filled------
	float distance; // distance from ray start point to the first interaction point
	float angle;
	Vec3f norm;	//normal of the interact surface
	Vec3f refl;

	Hit(Vec3f point, float distance, float angle, Vec3f norm, Vec3f refl):point(point), distance(distance), angle(angle),norm(norm),refl(refl), isHit(true) {};
};

}


#endif /* CORE_RAYHITSTRUCTS_H_ */
