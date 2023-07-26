/*
 * Camera.h
 *
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include "rapidjson/document.h"
#include "math/geometry.h"
#include "core/RayHitStructs.h"
#include <random>

using namespace rapidjson;

namespace rt{

class Camera{

protected:

	//
	//camera members
	//
	int height;
	int width;
	int fov; //field of view
	int samplesize = 1;
	std::string sampler;

	Vec3f position;
	Vec3f lookat;
	Vec3f up;

public:

	//
	// Constructors
	//
	Camera(){};
	Camera(int height, int width, int fov,  Vec3f position, Vec3f lookat, Vec3f up, int samplesize, std::string sampler):height(height), width(width), fov(fov),  position(position), lookat(lookat),up(up), samplesize(samplesize), sampler(sampler){};

	//
	// Destructor
	//
	virtual ~Camera();


	//
	// factory function : returns camera instance dynamically based on camera specifications
	//
	static Camera* createCamera(Value& cameraSpecs);
	virtual Ray shootRay(int pixelNumber, std::string sampler, int samplesize, int currentsample)=0;


	//
	// print function (to be implemented by the subclasses )
	//
	virtual void printCamera()=0;

	//
	// other camera functions (to complete)
	//


	//
	// Getters and Setters
	//
	int getHeight() const {
		return height;
	}

	void setHeight(int height) {
		this->height = height;
	}

	int getWidth() const {
		return width;
	}

	void setWidth(int width) {
		this->width = width;
	}

	int getFOV() const {
		return fov;
	}

	void setFOV(int fov) {
		this->fov = fov;
	}

	Vec3f getPosition() const {
		return position;
	}

	void setPosition(Vec3f position) {
		this->position = position;
	}

	Vec3f getLookat() const {
		return lookat;
	}

	void setLookat(Vec3f lookat) {
		this->lookat = lookat;
	}
	
	Vec3f getUp() const {
		return up;
	}

	void setUp(Vec3f up) {
		this->up = up;
	}

	int getSamplesize(){
		return samplesize;
	}

	std::string getSampler(){
		return sampler;
	}

};

} //namespace rt



#endif /* CAMERA_H_ */
