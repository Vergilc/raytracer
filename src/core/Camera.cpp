/*
 * Camera.cpp
 *
 */
#include <iostream>

#include "Camera.h"
#include "cameras/Pinhole.h"
#include "cameras/ThinLens.h"


namespace rt{

	Camera::~Camera(){};



/**
 * Factory function that returns camera subclass based on camera specifications
 *
 * @param cameraSpecs camera specifications json object
 *
 * @return camera subclass instance
 *
 */
Camera* Camera::createCamera(Value& cameraSpecs){

	//check if cameratype is defined

	if (!cameraSpecs.HasMember("type")){
		std::cerr<<"Camera type not specified"<<std::endl;
		exit(-1);
	}

	std::string cameraType=cameraSpecs["type"].GetString();
	Vec3f pos = Vec3f(cameraSpecs["position"].GetArray()[0].GetFloat(),cameraSpecs["position"].GetArray()[1].GetFloat(),cameraSpecs["position"].GetArray()[2].GetFloat());
	Vec3f lookat = Vec3f(cameraSpecs["lookat"].GetArray()[0].GetFloat(),cameraSpecs["lookat"].GetArray()[1].GetFloat(),cameraSpecs["lookat"].GetArray()[2].GetFloat());
	Vec3f up = Vec3f(cameraSpecs["up"].GetArray()[0].GetFloat(),cameraSpecs["up"].GetArray()[1].GetFloat(),cameraSpecs["up"].GetArray()[2].GetFloat());
	int w = cameraSpecs["width"].GetInt();
	int h = cameraSpecs["height"].GetInt();
	int fov = cameraSpecs["fov"].GetInt();
	
	int samplesize = 1;
	if (cameraSpecs.HasMember("samplesize")){
		samplesize = cameraSpecs["samplesize"].GetInt();
	}

	std::string sampler;
	if (cameraSpecs.HasMember("sampler")){
		sampler = cameraSpecs["sampler"].GetString();
	}
	else{
		sampler = "random";
	}

	//return camera object based on camera specs
	if (cameraType.compare("pinhole")==0){
		return new Pinhole(w,h,fov,pos, lookat, up, samplesize, sampler);
	}else if (cameraType.compare("thinlens")==0){

		if (!cameraSpecs.HasMember("focal")){
			std::cerr<<"Focal length not specified for thinlen camera! Creating Pinhole camera instead!"<<std::endl;
			return new Pinhole(w,h,fov,pos, lookat, up, samplesize, sampler);
		}
		float focal = cameraSpecs["focal"].GetFloat();

		if (!cameraSpecs.HasMember("d")){
			std::cerr<<"Distance from lens to image not specified for thinlen camera! Creating Pinhole camera instead!"<<std::endl;
			return new Pinhole(w,h,fov,pos, lookat, up, samplesize, sampler);
		}
		float d = cameraSpecs["d"].GetFloat();

		if (!cameraSpecs.HasMember("aperture")){
			std::cerr<<"Aperture not specified for thinlen camera! Creating Pinhole camera instead!"<<std::endl;
			return new Pinhole(w,h,fov,pos, lookat, up, samplesize, sampler);
		}
		float aperture = cameraSpecs["aperture"].GetFloat();
		
		return new ThinLens(w,h,fov, pos, lookat, up, focal, d, aperture, samplesize,sampler);
	}

	return 0;

}




} //namespace rt
