/*
 * Triangle.h
 *
 *
 */

#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "core/Shape.h"

namespace rt
{

	class Triangle : public Shape
	{
	public:
		//
		// Constructors
		//
		Triangle();
		Triangle(std::string id, Material *material, Vec3f v0, Vec3f v1, Vec3f v2);

		~Triangle();

		//
		// Functions that need to be implemented, since Triangle is a subclass of Shape
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

		boundingBox getBound();

		Vec3f getRandomPoint();
		
	private:
		Vec3f v0;
		Vec3f v1;
		Vec3f v2;
	};

} // namespace rt

#endif /* TRIANGLE_H_ */
