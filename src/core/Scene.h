/*
 * Scene.h
 *
 */

#ifndef SCENE_H_
#define SCENE_H_

#include <vector>

#include "rapidjson/document.h"
#include "core/LightSource.h"
#include "core/Shape.h"
#include "shapes/BVH.h"

using namespace rapidjson;

namespace rt{

class Scene {
public:

	Scene(){};
	~Scene();

	void createScene(Value& scenespecs);

	std::pair<Hit,Shape*> findHit(Ray Ray);

	Shape* findShape(std::string shapeid);

	std::vector<LightSource*> getLights(){
		return lightSources;
	};

	std::vector<Shape*> getShapes(){
		return shapes;
	};

	Vec3f getBGcolor(){
		return bgcolor;
	};

private:
	Vec3f bgcolor;
	std::vector<LightSource*> lightSources;
	std::vector<Shape*> shapes;
	BVH* bvh;
};

} //namespace rt



#endif /* SCENE_H_ */
