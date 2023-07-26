/*
 * Plane.h
 *
 *
 */

#ifndef PLANE_H_
#define PLANE_H_

#include "core/Shape.h"

namespace rt
{

	class Plane : public Shape
	{

	public:
		//
		// Constructors
		//
		Plane();
		Plane(std::string id, Material *material, Vec3f v0, Vec3f v1, Vec3f v2, Vec3f v3);

		~Plane();

		//
		// Functions that need to be implemented, since Plane is a subclass of Shape
		//
		Hit intersect(Ray ray);
		Vec3f getShadowColor(Ray incomingray, Vec3f lightpos, Vec3f lightid, Vec3f lightis,  Hit hitPoint, JPGLoader *loader);
		Vec3f getDistributedDiffColor(Ray incomingray, Vec3f raydir,  Vec3f lightcolor, Hit hitPoint, JPGLoader *loader);
		Vec3f getDistributedSpecColor(Ray incomingray, Vec3f raydir,  Vec3f lightcolor, Hit hitPoint, JPGLoader *loader);

		Vec3f getV0(){
			return this->v0;
		}

		Vec3f getV1(){
			return this->v1;
		}

		Vec3f getV2(){
			return this->v2;
		}

		Vec3f getV3(){
			return this->v3;
		}

		boundingBox getBound();

		Vec3f getRandomPoint();


	private:
		Vec3f v0;
		Vec3f v1;
		Vec3f v2;
		Vec3f v3;
	};

} // namespace rt

#endif /* PLANE_H_ */
