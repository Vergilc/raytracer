/*
 * Sphere.h
 *
 *
 */

#ifndef SPHERE_H_
#define SPHERE_H_

#include "core/Shape.h"

namespace rt
{

	class Sphere : public Shape
	{

	public:
		//
		// Constructors
		//
		Sphere();
		Sphere(std::string id, Material *material, Vec3f center, float radius);

		~Sphere();

		//
		// Functions that need to be implemented, since Sphere is a subclass of Shape
		//
		Hit intersect(Ray ray);
		Vec3f getShadowColor(Ray incomingray, Vec3f lightpos, Vec3f lightid, Vec3f lightis,  Hit hitPoint, JPGLoader *loader);
		Vec3f getDistributedDiffColor(Ray incomingray, Vec3f raydir,  Vec3f lightcolor, Hit hitPoint, JPGLoader *loader);
		Vec3f getDistributedSpecColor(Ray incomingray, Vec3f raydir,  Vec3f lightcolor, Hit hitPoint, JPGLoader *loader);

		Vec3f getCenter(){
			return this->center;
		}

		float getRadius(){
			return this->radius;
		}

		boundingBox getBound();

		Vec3f getRandomPoint();


	private:
		Vec3f center;
		float radius;
	};

} // namespace rt

#endif /* SPHERE_H_ */
