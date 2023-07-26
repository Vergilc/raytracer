/*
 * ThinLens.cpp
 *
 *
 */
#include <iostream>


#include "ThinLens.h"




namespace rt{

	//
	// ThinLens constructor (example)
	//
	ThinLens::ThinLens(int height, int width, int fov,  Vec3f position, Vec3f lookat, Vec3f up, float focal, float d, float aperture, int samplesize, std::string sampler){
		this->height = height;
		this->width = width;
		this->fov = fov;
		this->position = position;
		this->lookat = lookat.normalize();
		this->up = up;
		this->focal = focal;
		this->d = d;
		this->aperture = aperture;
		this->samplesize = samplesize;
		this->sampler = sampler;

		Vec3f dir = lookat;
		if ((up.normalize().crossProduct(dir)).length() == 0)
		{
			std::cerr<<"Error: up direction invalid!";
			exit(1);
		}
		right = up.normalize().crossProduct(dir).normalize();
		newUp = dir.crossProduct(right).normalize();
		float fovf = fov * 0.5f;
		fovf = fovf / 180.0f * 3.14159f;
		float tan_1 = 1 / tan(fovf);
		float dist = width / tan(fovf) / 2.0f;
		centre = position + dir * (-dist);
	}

	/**
	 * Prints camera data
	 * this function implements the abstract function of the base Camera class
	 */
	void ThinLens::printCamera(){
		printf("I am a thin lens camera! \n");
		printf("width: %dpx, height: %dpx, fov:%d \n", width, height, fov);
		printf("focal: %f, d: %f, aperture: f/%f \n", focal, d, aperture);
	}

	Ray ThinLens::shootRay(int pixelNumber, std::string sampler, int samplesize, int currentsample){

		//std::cout<<"Camera position is: "<< position.x <<", "<< position.y <<", "<< position.z << "\n";
		//std::cout<<"Camera screen center is: "<< centre.x <<", "<< centre.y <<", "<< centre.z << "\n";
		// std::default_random_engine generator;
		// std::uniform_real_distribution uniform_random;
		// std::normal_distribution gaussian_jitter;

		float rand_u = (std::rand()* 1.0f) / RAND_MAX  - 0.5f;	// random sample u position from [-0.5,0.5]
		float rand_v = (std::rand()* 1.0f) / RAND_MAX  - 0.5f;	// random sample v position from [-0.5,0.5]

		if (sampler.compare("jittered") == 0){
			// uniformly jittered
			int ss = samplesize;
			int samplewidth = (int) sqrt(samplesize);
			int i_in_pixel = currentsample % samplewidth + 1;
			int j_in_pixel = currentsample / samplewidth + 1;

			float u_change_sampling = (1.0f + samplewidth) / 2.0f - i_in_pixel;
			float v_change_sampling = (1.0f + samplewidth) / 2.0f - j_in_pixel;
			float samplegap = 1.0f / (samplewidth);
			rand_u = (rand_u  + u_change_sampling) * samplegap;
			rand_v = (rand_v  + v_change_sampling) * samplegap;
		} 

		int i = pixelNumber;
		// u for x-axis and v for y-axis on screen
		int ui = i % width + 1;
		float u_change = (width + 1.0f) / 2.0f - ui - rand_u;
		int vi = i / height + 1;
		float v_change = (height + 1.0f) / 2.0f - vi - rand_v;
		Vec3f des = centre + right * (-u_change) + newUp * (-v_change);
		float ratio = d / (des - position).length();
		des = position + (des - position).normalize() * d;
		// calculating refraction in lens
		float angle = 2.0f * std::_Pi * std::rand() / RAND_MAX;	// random ray angle from [0, 2 * pi], decide the direction from center to hitpoint on thinlens
		float u_diff = cos(angle);
		float v_diff = sin(angle);
		float r = focal / aperture / 2;
		float bias = r * std::rand() / RAND_MAX;	// random ray position from [0, r], decide the distance from hitpoint to the center of thinlens
		Vec3f lenhit = position + right * (u_diff) * bias + newUp * (v_diff) * bias;
		float u_r = (u_diff*bias + u_change*ratio) * focal / d;
		float v_r = (v_diff*bias + v_change*ratio) * focal / d;
		Vec3f focalPlanePos = position + lookat * focal + right * u_r + newUp * v_r;
		//std::cout<<"Camera ray des is: "<< des.x <<", "<< des.y <<", "<< des.z << "\n";
		Ray newray = Ray(lenhit, focalPlanePos - lenhit, PRIMARY);
		return newray;
	}

} //namespace rt
