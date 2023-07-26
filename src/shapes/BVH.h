/*
 * BVH.h
 *
 *
 */

#ifndef BVH_H_
#define BVH_H_

#include <vector>
#include "core/Shape.h"

namespace rt
{

	class BVH : public Shape
	{
	public:
		//
		// Constructors
		//
		BVH();
		BVH(std::string id, boundingBox box);

		static BVH *createBVH(std::vector<Shape*> shapes);

		~BVH();

		//
		// Functions that need to be implemented, since Sphere is a subclass of Shape
		//
		Hit intersect(Ray ray);

		std::pair<Hit, Shape*> findHit(Ray ray);

		Vec3f getShadowColor(Ray incomingray, Vec3f lightpos, Vec3f lightid, Vec3f lightis,  Hit hitPoint, JPGLoader *loader);
		Vec3f getDistributedDiffColor(Ray incomingray, Vec3f raydir,  Vec3f lightcolor, Hit hitPoint, JPGLoader *loader){return Vec3f(0,0,0);};
		Vec3f getDistributedSpecColor(Ray incomingray, Vec3f raydir,  Vec3f lightcolor, Hit hitPoint, JPGLoader *loader){return Vec3f(0,0,0);};

		boundingBox getBound();

		Vec3f getRandomPoint();

	private:
		Shape* shape;

		boundingBox boundingbox;

		BVH *lbvh, *rbvh;
	};


} // namespace rt

#endif /* BVH_H_ */
