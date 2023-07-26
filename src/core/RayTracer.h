/*
 * RayTracer.h
 *
 */
#ifndef RAYTRACER_H_
#define RAYTRACER_H_

#include "math/geometry.h"
#include "core/Camera.h"
#include "core/Scene.h"
#include <random>

namespace rt{

/*
 * Raytracer class declaration
 */
class RayTracer {
public:

	RayTracer(){};

	static Vec3f* render(Camera* camera, Scene* scene, int nbounces);

	static Vec3f* render_ss(Camera* camera, Scene* scene, int nbounces, int samplesize, std::string sampleing);

	static Vec3f trace(Ray incomingray, Scene* scene, int nbounces, bool distri,JPGLoader *loader);

	static Vec3f* tonemap(Vec3f* pixelbuffer, Camera* camera);

	static Ray getReflectRay(Ray incomingray, Hit Hit);

	static Ray getRefractRay(Ray incomingray, Hit hitPoint, float theta);

	static Ray getDistributedDiff(Ray incomingray, Hit hitPoint);

	static Ray getDistributedSpec(Ray incomingray, Hit hitPoint, float spec);

private:


};




} //namespace rt



#endif /* RAYTRACER_H_ */

