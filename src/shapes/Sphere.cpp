/*
 * Sphere.cpp
 *
 *
 */
#include "Sphere.h"

namespace rt
{

	Sphere::Sphere(std::string id, Material *material, Vec3f center, float radius)
	{
		this->id = id;
		this->material = material;
		this->center = center;
		this->radius = radius;
	}

	/**
	 * Computes whether a ray hit the specific instance of a sphere shape and returns the hit data
	 *
	 * @param ray cast ray to check for intersection with shape
	 *
	 * @return hit struct containing intersection information
	 *
	 */
	Hit Sphere::intersect(Ray ray)
	{
		Hit h = Hit();
		//-----------to be implemented -------------

		Vec3f origin = ray.origin;
		Vec3f dir = ray.direction;
		Vec3f anotherpoint = origin + dir;

		RayType raytype = ray.raytype;

		// Find the vector along the line
		Vec3f v = dir;
		// Find the vector from one point on the line to the given point
		Vec3f w = center - origin;
		// Find the dot product of v and w
		float dot = dir.dotProduct(w);
		// Find the parameter t that minimizes the distance
		float t = dot / v.norm();
		// Find the closest point on the line
		Vec3f cloest = origin + t * v;
		// Return the distance between c and p
		float dist = (cloest - center).length();
		if (dist > radius)	return h;
		// |origin + t * dir - center| = radius
		Vec3f z = origin - center; // z = origin - center, as a constant value
		float a = dir.norm();
		float b = 2 * z.dotProduct(dir);
		float c = z.norm() - radius * radius;

		float t1 = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
		float t2 = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
		t = -1;
		Vec3f hitPoint;

		if (t1 > 0)
		{
			if (t1 > t2 && t2 > 0) t = t2;
			else t = t1;
		}
		else if (t2 > 0) t = t2;

		if (t > 0)
		{
			hitPoint = origin + t * dir;
			float distance = dir.length() * t; // distance from ray start point to the first interaction point
			Vec3f normal = (hitPoint - center).normalize();
			if (normal.dotProduct(dir) > 0) normal = - normal;
			// angle = arccos(a * b) / (a.len * b.len)
			Vec3f projection = normal * (normal.dotProduct(dir));
			Vec3f reflectiondir = dir - 2 * projection;

			float angle = acos((-dir.normalize()).dotProduct(normal));
			h = Hit(hitPoint, distance, angle, normal, reflectiondir);
			// printf("Found a Hit on sphere with angle: %f \n", angle);
		}
		return h;
	}

	Vec3f Sphere::getShadowColor(Ray incomingray,Vec3f lightpos, Vec3f lightid, Vec3f lightis,  Hit hitPoint, JPGLoader *loader)
	{
		Vec3f diff = material->getDiffuseColor(incomingray, lightpos,lightid, hitPoint);
		if (!material->tPath._Equal(""))
		{
			int w = material->tWidth;
			int h = material->tHeight;
			Vec3f C = center;
			float r = radius;
			Vec3f P = hitPoint.point;

			Vec3f dir = P - C;

			// using the top point as (0,0) in texture,
			// using the curve on top of x-axis as the left bounary of the texture (0,v)
			float angle_u = acos(dir.x / r);
			float u = angle_u / 3.14159f;
			if (dir.y < 0) u = -u;
			float angle_v = acos(dir.z / r);
			float v = angle_v / 3.14159f;

			u = u * w;
			int u_valid = u;
			while (u_valid > w){u_valid -= w;}
			while (u_valid < 0){u_valid += w;}
			v = v * h;
			int v_valid = v;
			while (v_valid > h){v_valid -= h;}
			while (v_valid < 0){v_valid += h;}

			Vec3f pixelcolor = loader->getValue(material->tPath, w, h,u_valid, v_valid);
			diff = pixelcolor * (diff.length() / material->diffusecolor.length());
		}
		Vec3f spec = material->getSpecularColor(incomingray, lightpos,lightis, hitPoint);
		Vec3f res = diff + spec;
		res.x = std::max(res.x, 0.0f);
		res.y = std::max(res.y, 0.0f);
		res.z = std::max(res.z, 0.0f);
		return res;
	}

	Vec3f Sphere::getDistributedDiffColor(Ray incomingray, Vec3f raydir,  Vec3f lightcolor, Hit hitPoint, JPGLoader *loader)
	{
		Vec3f diff = material->getDistributedDiff(incomingray, raydir,lightcolor, hitPoint);
		if (!material->tPath._Equal(""))
		{
			int w = material->tWidth;
			int h = material->tHeight;
			Vec3f C = center;
			float r = radius;
			Vec3f P = hitPoint.point;
			Vec3f dir = P - C;
			// using the top point as (0,0) in texture,
			// using the curve on top of x-axis as the left bounary of the texture (0,v)
			float angle_u = acos(dir.x / r);
			float u = angle_u / 3.14159f;
			if (dir.y < 0) u = -u;
			float angle_v = acos(dir.z / r);
			float v = angle_v / 3.14159f;

			u = u * w;
			int u_valid = u;
			while (u_valid > w){u_valid -= w;}
			while (u_valid < 0){u_valid += w;}
			v = v * h;
			int v_valid = v;
			while (v_valid > h){v_valid -= h;}
			while (v_valid < 0){v_valid += h;}

			Vec3f pixelcolor = loader->getValue(material->tPath, w, h,u_valid, v_valid);
			diff = pixelcolor * (diff.length() / material->diffusecolor.length());
		}
		Vec3f res = diff;
		res.x = std::max(res.x, 0.0f);
		res.y = std::max(res.y, 0.0f);
		res.z = std::max(res.z, 0.0f);
		return res;
	}

	Vec3f Sphere::getDistributedSpecColor(Ray incomingray, Vec3f raydir,  Vec3f lightcolor, Hit hitPoint, JPGLoader *loader)
	{
		Vec3f res = material->getDistributedSpec(incomingray,raydir,lightcolor,hitPoint);
		res.x = std::max(res.x, 0.0f);
		res.y = std::max(res.y, 0.0f);
		res.z = std::max(res.z, 0.0f);
		return res;
	}

	boundingBox Sphere::getBound(){
		float xMax, xMin, yMax, yMin, zMax, zMin;
		xMax = center.x + radius;
		xMin = center.x - radius;
		yMax = center.y + radius;
		yMin = center.y - radius;
		zMax = center.z + radius;
		zMin = center.z - radius;

		return boundingBox(xMax, xMin, yMax, yMin, zMax, zMin);
	}
	
	Vec3f Sphere::getRandomPoint(){
		std::default_random_engine generator;
		std::normal_distribution gaussian_jitter;
		float x = gaussian_jitter(generator);
		float y = gaussian_jitter(generator);
		float z = gaussian_jitter(generator);

		Vec3f dir(x,y,z);
		dir.normalize();
		return center + dir * radius;
	}

	Sphere::~Sphere()
	{
		delete this->material;
	}

} // namespace rt
