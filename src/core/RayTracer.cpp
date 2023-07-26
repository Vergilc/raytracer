/*
 * RayTracer.cpp
 *
 */
#include "RayTracer.h"
#include "lights/AreaLight.h"
#include <execution>
#include <algorithm>

namespace rt
{
	Ray RayTracer::getReflectRay(Ray incomingray, Hit hitPoint)
	{
		Vec3f normal = hitPoint.norm;
		normal.normalize();
		Vec3f dir = incomingray.direction;
		dir.normalize();
		float diff = (normal.dotProduct(-dir));
		Vec3f r = (normal * diff * 2 + dir).normalize();
		Vec3f newStart = hitPoint.point + r * 0.00001f; // adding 0.0001 to avoid hitting source object
		Ray newray = Ray(newStart + normal * 0.00001f, r, SECONDARY);
		if (incomingray.raytype == REFRACT)
			newray.raytype = REFRACT;
		return newray;
	}

	Ray RayTracer::getDistributedDiff(Ray incomingray, Hit hitPoint)
	{
		Vec3f normal = hitPoint.norm;
		normal.normalize();
		Vec3f dir = incomingray.direction;
		dir = -dir.normalize();
		Vec3f u_dir = (normal.crossProduct(dir)).normalize();
		Vec3f v_dir = (normal.crossProduct(u_dir)).normalize();

		float dir_u = std::rand() / (RAND_MAX * 1.0f) - 0.5f;	// random distributed direction with u position from [-0.5,0.5]
		float dir_v = std::rand() / (RAND_MAX * 1.0f) - 0.5f;	// random distributed direction with u position from [-0.5,0.5]
		float dir_n = (std::rand() + 1.0f) / (RAND_MAX * 2.0f); // random distributed direction in normal direction from (0,0.5]

		Vec3f r = normal * dir_n + u_dir * dir_u + v_dir * dir_v;
		r.normalize();
		Vec3f newStart = hitPoint.point + r * 0.00001f;
		Ray newray = Ray(newStart + normal * 0.00001f, r, SECONDARY);
		if (incomingray.raytype == REFRACT)
			newray.raytype = REFRACT;
		return newray;
	}

	Ray RayTracer::getDistributedSpec(Ray incomingray, Hit hitPoint, float spec)
	{
		Vec3f normal = hitPoint.norm;
		normal.normalize();
		Vec3f dir = hitPoint.refl;
		dir = dir.normalize();
		Vec3f u_dir = (normal.crossProduct(dir)).normalize();
		Vec3f v_dir = (dir.crossProduct(u_dir)).normalize();
		Vec3f r;
		while (r.dotProduct(normal) < 0)
		{
			std::default_random_engine generator;
			std::normal_distribution<float> distribution;
			float dir_u = pow(distribution(generator), spec); // normally distributed around the perfectly specular ray
			float dir_v = pow(distribution(generator), spec); // normally distributed around the perfectly specular ray
			r = hitPoint.refl + u_dir * dir_u + v_dir * dir_v;
			r.normalize();
		}
		Vec3f newStart = hitPoint.point + r * 0.00001f;
		Ray newray = Ray(newStart + normal * 0.00001f, r, SECONDARY);
		if (incomingray.raytype == REFRACT)
			newray.raytype = REFRACT;

		return newray;
	}

	Ray RayTracer::getRefractRay(Ray incomingray, Hit hitPoint, float eta)
	{

		Vec3f incoming = incomingray.direction;
		incoming = incoming.normalize();
		Vec3f norm = hitPoint.norm;
		norm.normalize();

		if (incomingray.raytype == REFRACT)
			eta = 1.f / eta;

		float cos_theta_i = incoming.dotProduct(norm);
		float sin_theta_i_2 = 1.0f - cos_theta_i * cos_theta_i;
		float sin_theta_r_2;

		// Here ignore the case of refraction between two materials other than air
		sin_theta_r_2 = sin_theta_i_2 * (1.0f / eta / eta);

		if (sin_theta_r_2 > 1.0f)
		{
			// Total internal reflection, no refraction ray
			return Ray();
		}

		float cos_theta_r = sqrt(1.0f - sin_theta_r_2);
		Vec3f newdir = incoming * (1.f / eta) + norm * (-cos_theta_r - cos_theta_i / eta);
		newdir.normalize();
		if (incomingray.raytype == REFRACT)
		{
			return Ray(hitPoint.point + newdir * 0.00001f, newdir, SECONDARY);
		}
		else
		{
			return Ray(hitPoint.point + newdir * 0.00001f, newdir, REFRACT);
		}
	}

	Vec3f RayTracer::trace(Ray incomingray, Scene *scene, int nbounces, bool isDistributed, JPGLoader *loader)
	{
		if (incomingray.raytype == BADRAY)
			return Vec3f(0, 0, 0);
		int n = nbounces;
		// printf("Finding next bounce.\n");
		Vec3f color = scene->getBGcolor();
		if (n == 0)
			return color;
		auto tup = scene->findHit(incomingray);
		Hit nextHit = tup.first;
		if (!nextHit.isHit)
		{
			return color;
		} // return background color
		else
		{
			float multi = 10.f / (10.f + nextHit.distance);
			Shape *hitedshape = tup.second;
			std::vector<LightSource *> lights = scene->getLights();
			if (hitedshape->checkislight())
			{
				// If ray hit an arealight, return the light_is modified by distance
				for (auto light : lights)
				{
					if (light->shapeid._Equal(hitedshape->getId()))
					{
						if (incomingray.raytype == PRIMARY)
						{
							return color + light->is * (255.0f / 100.0f);
						}
						return color + light->is * multi * (255.0f / 100.0f);
					}
				}
				printf("Warning: a shape is marked as arealight, but no light matches its shape id!\n");
			}
			Material *material = hitedshape->getMaterial();

			for (int i = 0; i < lights.size(); i++)
			{
				LightSource *light = lights[i];
				std::string shapeid = light->shapeid;
				if (shapeid._Equal(""))
				{
					// Point Light
					Vec3f newDir = light->position - nextHit.point;
					if (newDir.dotProduct(nextHit.norm) <= 0)
						continue;
					Vec3f hitPosition = nextHit.point + nextHit.norm * 0.0001f; // adding 0.0001 to avoid hitting source object
					Ray lightRay(hitPosition, newDir, SHADOW);
					std::pair<Hit, Shape *> blockpair = scene->findHit(lightRay);
					Hit blockhit = blockpair.first;
					bool shadow = true;
					if (!blockhit.isHit)
						shadow = false;
					else
					{
						// printf("block hit distance: %f, light distance is %f \n", blockhit.distance, newDir.length());
						if ((blockhit.distance + 0.0002f) > newDir.length())
							shadow = false;
					}
					if (!shadow)
					{
						// printf("Calculating shadow light\n");
						Vec3f shadowcolor = hitedshape->getShadowColor(incomingray, light->position, light->id, light->is, nextHit, loader);
						color = color + shadowcolor * 1.f;
					}
				}
				else
				{
					Shape *lightshape = scene->findShape(shapeid);
					if (lightshape == nullptr)
					{
						printf("Error: AreaLight shape not found!\n");
					}
					else if (!lightshape->checkislight())
					{
						printf("Error: AreaLight shape not marked!\n");
					}
					else
					{
						int areasamplesize = 1;
						for (int i = 0; i < areasamplesize; i++)
						{
							Vec3f lightpos = lightshape->getRandomPoint();
							Vec3f dir = lightpos - nextHit.point;
							if (dir.dotProduct(nextHit.norm) <= 0)
								continue;
							Ray newray(nextHit.point + nextHit.norm * 0.00001f, dir, SHADOW);
							auto res = scene->findHit(newray);
							Hit blockhit = res.first;
							Shape *blockshape = res.second;
							bool shadow = true;
							if (!blockhit.isHit)
								shadow = false;
							else
							{
								if ((blockhit.distance + 0.0001f) > dir.length())
									shadow = false;
								// if the blocked shape is the arealight
								else if (blockshape->getId()._Equal(lightshape->getId()))
									shadow = false;
							}
							if (!shadow)
							{
								Vec3f shadowcolor = hitedshape->getShadowColor(incomingray, lightpos, light->id, light->is, nextHit, loader);
								color = color + shadowcolor * 1.f * (1.f / areasamplesize);
							}
						}
					}
				}
			}
			if (n > 0)
			{
				// Only use distributed ray-tracing on non-refract rays
				if (isDistributed && incomingray.raytype != REFRACT)
				{
					// for (int j = 0; j < num; j ++){
					Ray distributedDiff = getDistributedDiff(incomingray, nextHit);
					Vec3f diffcolor = trace(distributedDiff, scene, n - 1, isDistributed, loader);
					Ray distributedSpec = getDistributedSpec(incomingray, nextHit, hitedshape->getMaterial()->specularexponent);
					Vec3f speccolor = trace(distributedSpec, scene, n - 1, isDistributed, loader);
					// Vec3f refractcolor = getColor(reflectRay, scene, nbounces);
					color = color + hitedshape->getDistributedDiffColor(incomingray, distributedDiff.direction, diffcolor, nextHit, loader);
					color = color + hitedshape->getDistributedSpecColor(incomingray, distributedSpec.direction, speccolor, nextHit, loader);
					// }
				}
				// Ensure main reflection ray
				if (hitedshape->getMaterial()->isReflective())
				{
					Ray reflectRay = getReflectRay(incomingray, nextHit);
					Vec3f reflectcolor = trace(reflectRay, scene, n - 1, isDistributed, loader);
					// Vec3f refractcolor = getColor(reflectRay, scene, nbounces);
					color = color + reflectcolor * material->kr;
				}
				if (hitedshape->getMaterial()->istransparent())
				{
					// printf("Getting transmitted ray\n");
					Ray refractRay = getRefractRay(incomingray, nextHit, hitedshape->getMaterial()->eta);
					if (refractRay.raytype != BADRAY)
					{
						Vec3f refractcolor = trace(refractRay, scene, n - 1, isDistributed, loader);
						color = color + refractcolor * material->kt;
					}
				}
			}
			// color = Shade(ReflectColor, RefractColor, nextHit);
			// Only perform distance-correction on secondary rays
			if (incomingray.raytype != PRIMARY)
				color = color * multi;
		}
		return color;
	}

	/**
	 * Performs ray tracing to render a photorealistic scene
	 *
	 * @param camera the camera viewing the scene
	 * @param scene the scene to render, including objects and lightsources
	 * @param nbounces the number of bounces to consider for raytracing
	 *
	 * @return a pixel buffer containing pixel values in linear RGB format
	 */
	Vec3f *RayTracer::render(Camera *camera, Scene *scene, int nbounces)
	{
		int w = camera->getWidth();
		int h = camera->getHeight();
		int samplesize = camera->getSamplesize();
		std::string sampler = camera->getSampler();

		int totalpixels = w * h;
		Vec3f *pixelbuffer = new Vec3f[totalpixels];
#ifdef LOG
		printf("Rendering pixel buffer with length: %d \n", totalpixels);
#endif
		printf("Loading necessary textures\n");
		JPGLoader *loader = JPGLoader::createJPGLoader();

		for (auto shape : scene->getShapes())
		{
			if (!shape->getMaterial()->tPath._Equal(""))
			{
				loader->loadJPG(shape->getMaterial()->tPath);
			}
		}
		printf("All texture loaded\n");
		std::for_each(std::execution::par_unseq, pixelbuffer, pixelbuffer + totalpixels, [&](Vec3f &pixel)
					  {
			int i = (&pixel) - pixelbuffer;

			int wid = camera->getWidth();
			int w = i % wid;
			int h = i / wid;

			Vec3f color(0,0,0);
			for (int j = 0; j < samplesize; j ++){
				Ray newray = camera->shootRay(i, sampler, samplesize, j);
				// set maximun 1 bounce for distributed ray tracing
				Vec3f newcolor = trace(newray, scene, nbounces + 1, true, loader);
				color = color + newcolor * (1.f/samplesize);
			}
			
			float max_c = std::max(std::max(color.x, color.y), color.z);
			if (max_c > 1) color /= max_c;
			pixel = color; });

		printf("Rendering finished!\n");
		delete loader;
		printf("JPGLoader deleted\n");
		return pixelbuffer;
	}

	/**
	 * Performs ray tracing to render a photorealistic scene, with sample size and sampling metod specified
	 *
	 * @param camera the camera viewing the scene
	 * @param scene the scene to render, including objects and lightsources
	 * @param nbounces the number of bounces to consider for raytracing
	 *
	 * @return a pixel buffer containing pixel values in linear RGB format
	 */
	Vec3f *RayTracer::render_ss(Camera *camera, Scene *scene, int nbounces, int samplesize, std::string sampling)
	{
		int w = camera->getWidth();
		int h = camera->getHeight();

		int totalpixels = w * h;
		Vec3f *pixelbuffer = new Vec3f[totalpixels];
#ifdef LOG
		printf("Rendering pixel buffer with length: %d \n", totalpixels);
#endif
		printf("Loading necessary textures\n");
		JPGLoader *loader = JPGLoader::createJPGLoader();

		for (auto shape : scene->getShapes())
		{
			if (!shape->getMaterial()->tPath._Equal(""))
			{
				loader->loadJPG(shape->getMaterial()->tPath);
			}
		}
		std::for_each(std::execution::par_unseq, pixelbuffer, pixelbuffer + totalpixels, [&](Vec3f &pixel)
					  {
			int i = (&pixel) - pixelbuffer;

			int wid = camera->getWidth();
			int w = i % wid;
			int h = i / wid;

			// if (w < 400 || w > 700 || h < 400 || h > 750){
			// 	return scene->getBGcolor();
			// }

			Vec3f color(0,0,0);

			int samplewidth = (int) sqrt(samplesize);
			samplesize = samplewidth * samplewidth;

			for (int j = 0; j < samplesize; j ++){
				Ray newray = camera->shootRay(i, sampling, samplesize, j);
				// set maximun 1 bounce for distributed ray tracing
				Vec3f newcolor = trace(newray, scene, nbounces + 1, true, loader);
				color = color + newcolor * (1.f/samplesize);
			}
			
			float max_c = std::max(std::max(color.x, color.y), color.z);
			if (max_c > 1) color /= max_c;
			pixel = color; });

		printf("Rendering finished!\n");
		delete loader;
		printf("JPGLoader deleted\n");
		return pixelbuffer;
	}

	/**
	 * Tonemaps the rendered image (conversion of linear RGB values [0-1] to low dynamic range [0-255]
	 *
	 * @param pixelbuffer the array of size width*height containing linear RGB pixel values
	 *
	 * @return the tonemapped image
	 */
	Vec3f *RayTracer::tonemap(Vec3f *pixelbuffer, Camera *camera)
	{
		//---------tonemapping function to be filled--------
		for (int i = 0; i < camera->getWidth() * camera->getHeight(); i++)
		{
			pixelbuffer[i] *= 255;
		}
		printf("Tonemapping finished.\n");
		return pixelbuffer;
	}

} // namespace rt
