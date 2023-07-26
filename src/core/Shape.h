/*
 * Shape.h
 *
 */

#ifndef SHAPE_H_
#define SHAPE_H_

#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <random>

#include "math/geometry.h"
#include "core/Material.h"
#include "materials/BlinnPhong.h"
#include "rapidjson/document.h"
#include "jpgloader/JPGLoader.h"


namespace rt{

using namespace rapidjson;

struct boundingBox
	{
		boundingBox(float xMax, float xMin, float yMax, float yMin, float zMax, float zMin) : xMax(xMax), xMin(xMin), yMax(yMax), yMin(yMin), zMax(zMax), zMin(zMin){};
		float xMax, xMin, yMax, yMin, zMax, zMin;
	};

boundingBox mergeBox(boundingBox lbox, boundingBox rbox);

bool contains(boundingBox mainbox, boundingBox newbox);

bool boxrayintersect(boundingBox box, Ray ray);

class Shape{
public:

	//
	// Constructors
	//
	Shape(){};
	Shape(Material* material, std::string id):material(material), id(id){};

	//
	// Destructor (must be overriden in subclass)
	//
	virtual ~Shape();


	static Shape* createShape(Value& shapeSpecs);

	virtual boundingBox getBound() = 0;

	//
	// Shape abstract methods (to be implemented by subclasses)
	//
	virtual Hit intersect(Ray)=0;

	virtual Vec3f getShadowColor(Ray incomingray,Vec3f lightpos, Vec3f lightid, Vec3f lightis, Hit nextHit, JPGLoader *loader) = 0;
	virtual Vec3f getDistributedDiffColor(Ray incomingray, Vec3f raydir,  Vec3f lightcolor, Hit hitPoint, JPGLoader *loader) = 0;
	virtual Vec3f getDistributedSpecColor(Ray incomingray, Vec3f raydir,  Vec3f lightcolor, Hit hitPoint, JPGLoader *loader) = 0;


	std::string getId(){
		return id;
	}

	Material* getMaterial(){
		return material;
	}

	virtual Vec3f getRandomPoint() = 0;

	void islight(){
		isLight = true;
	}

	bool checkislight(){
		return isLight;
	}

protected:
	Material * material;
	std::string id;
	bool isLight = false;
};


} //namespace rt


#endif /* SHAPE_H_ */
