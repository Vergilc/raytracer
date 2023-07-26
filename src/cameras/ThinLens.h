/*
 * ThinLens.h
 *
 */

#ifndef THINLENS_H_
#define THINLENS_H_

#include "core/Camera.h"


namespace rt{

class ThinLens:public Camera{
public:

	//
	// Constructors
	//
	ThinLens();
	ThinLens(int height, int width, int fov,  Vec3f position, Vec3f lookat, Vec3f up, float focal, float d, float aperture, int samplesize, std::string sampler);


	//
	//Destructor
	//
	~ThinLens(){};

	//
	// print function (implementing abstract function of base class)
	//
	void printCamera();
	Ray shootRay(int pixelNumber, std::string sampler, int samplesize, int currentsample);

private:
	float focal = 1.f;
	float d = 3.f;
	float aperture;
	Vec3f right;
	Vec3f newUp;
	Vec3f centre;
};



} //namespace rt




#endif /* THINLENS_H_ */
