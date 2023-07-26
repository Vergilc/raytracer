/*
 * Pinhole.cpp
 *
 *
 */
#include <iostream>

#include "Pinhole.h"

namespace rt
{

	//
	// Pinhole constructor
	//
	Pinhole::Pinhole(int height, int width, int fov, Vec3f position, Vec3f lookat, Vec3f up, int samplesize, std::string sampler)
	{
		this->height = height;
		this->width = width;
		this->fov = fov;
		this->position = position;
		this->lookat = lookat;
		this->up = up;
		this->samplesize = samplesize;
		this->sampler = sampler;

		Vec3f dir = lookat;
		dir = dir.normalize();
		if ((up.normalize().crossProduct(dir)).length() == 0)
		{
			std::cerr<<"Error: up direction invalid!";
			exit(1);
		}
		right = up.normalize().crossProduct(dir).normalize();
		newUp = dir.crossProduct(right).normalize();
		float fovf = fov * 0.5f / 180.0f * 3.14159f;
		float tan_1 = 1 / tan(fovf);
		float dist = width / tan(fovf) / 2.0f;
		centre = position + dir * (-dist);
	}

	// to fill

	/**
	 * Prints camera data
	 * this function implements the abstract function of the base Camera class
	 */
	void Pinhole::printCamera()
	{
		printf("I am a pinhole camera! \n");
		printf("width: %dpx, height: %dpx, fov:%d \n", width, height, fov);
		std::cout<<"Right: "<< right << ", newup: "<< newUp<< std::endl;

	}

	Ray Pinhole::shootRay(int pixelNumber, std::string sampler, int samplesize, int currentsample){

		//std::cout<<"Camera position is: "<< position.x <<", "<< position.y <<", "<< position.z << "\n";
		//std::cout<<"Camera screen center is: "<< centre.x <<", "<< centre.y <<", "<< centre.z << "\n";
		// std::default_random_engine generator;
		// std:: uniform_real_distribution uniform_random;
		// std:: normal_distribution gaussian_jitter;

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
		float u_change = (1.0f + width) / 2.0f - ui - rand_u;
		int vi = i / height + 1;
		float v_change = (1.0f + height) / 2.0f - vi - rand_v;

		Vec3f des = centre + right * (-u_change) + newUp * (-v_change);
		des = position + (des - position).normalize();
		return Ray(position, (position - des).normalize(), PRIMARY);
	}

} // namespace rt
