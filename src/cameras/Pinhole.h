/*
 * Pinhole.h
 *
 *
 */

#ifndef PINHOLE_H_
#define PINHOLE_H_

#include "core/Camera.h"

namespace rt{

class Pinhole:public Camera{
public:

	//
	// Constructors
	//
	Pinhole();
	Pinhole(int height, int width, int fov,  Vec3f position, Vec3f lookat, Vec3f up, int samplesize, std::string sampler);

	//
	//Destructor
	//
	~Pinhole(){};

	//
	// print function (implementing abstract function of base class)
	//
	void printCamera();

	Ray shootRay(int pixelNumber, std::string sampler, int samplesize, int currentsample);

private:

	Vec3f right;
	Vec3f newUp;
	Vec3f centre;
};



} //namespace rt




#endif /* PINHOLE_H_ */
