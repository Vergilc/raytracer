/*
 * Plane.cpp
 *
 *
 */
#include "Plane.h"

namespace rt
{
	Plane::Plane(std::string id, Material *material, Vec3f v0, Vec3f v1, Vec3f v2, Vec3f v3)
	{
		this->id = id;
		this->material = material;
		this->v0 = v0;
		this->v1 = v1;
		this->v2 = v2;
		this->v3 = v3;
	}

	/**
	 * Computes whether a ray hit the specific instance of a sphere shape and returns the hit data
	 *
	 * @param ray cast ray to check for intersection with shape
	 *
	 * @return hit struct containing intersection information
	 *
	 */
	Hit Plane::intersect(Ray ray)
	{

		Hit h = Hit();
		//-----------to be implemented -------------

		Vec3f origin = ray.origin;
		Vec3f dir = ray.direction;

		RayType tarytype = ray.raytype;

		// the while plane of ACBD: n * p + d = 0
		// n = AC X AB
		// AC = C - A
		// d = - n * A

		Vec3f AC = v2 - v0;
		Vec3f AB = v1 - v0;
		Vec3f norm = AC.crossProduct(AB);
		float d = -norm.dotProduct(v0);

		// ray = origin + t * dir

		// if n * r = 0: parallel, no interact or infinite intersection, ignore
		if (norm.dotProduct(dir) == 0)
		{
			return h;
		}

		// t = n * (A-O) / (n * r)

		float t = norm.dotProduct(v0 - origin) / norm.dotProduct(dir);
		if (t < 0)
		{
			return h;
		}

		Vec3f p = origin + dir * t; // point of intersection

		// check if inside Plane ABCD :
		// r1 = n * (AP X AB)
		float r1 = norm.dotProduct((p - v0).crossProduct(v1 - v0)); // PA BA
		float r2 = norm.dotProduct((p - v1).crossProduct(v2 - v1)); // PB CB
		float r3 = norm.dotProduct((p - v2).crossProduct(v3 - v2)); // PC DC
		float r4 = norm.dotProduct((p - v3).crossProduct(v0 - v3)); // PD AD
		float r5 = norm.dotProduct((p - v0).crossProduct(v2 - v0)); // PA CA
		float r6 = norm.dotProduct((p - v1).crossProduct(v3 - v1)); // PB DB
		float r7 = norm.dotProduct((p - v2).crossProduct(v0 - v2)); // PC AC
		float r8 = norm.dotProduct((p - v3).crossProduct(v1 - v3)); // PD BD

		bool abc = ((r1 * r2 > 0) && (r2 * r7 > 0) && (r1 * r7 > 0));
		bool abd = ((r1 * r6 > 0) && (r1 * r4 > 0) && (r6 * r4 > 0));
		bool acd = ((r5 * r3 > 0) && (r5 * r4 > 0) && (r3 * r4 > 0));
		bool bcd = ((r2 * r3 > 0) && (r2 * r8 > 0) && (r3 * r8 > 0));

		bool inter = false;

		if (abc || acd){
			if (abd || bcd) inter = true;
		}

		if (inter)
		{
			// intersection with plane ABCD at p confirmed

			float distance = (p - origin).length(); // distance from ray start point to the first interaction point
			dir = dir.normalize();
			norm = norm.normalize();
			// angle = arccos(a * b) / (a.len * b.len)
			if (dir.dotProduct(norm) > 0)
			{
				norm = -norm;
			}
			float angle = acos((-dir).dotProduct(norm));
			Vec3f projection = norm * (norm.dotProduct(dir));
			Vec3f reflectiondir = dir - 2.0f * projection;
			h = Hit(p, distance, angle, norm, reflectiondir);
			// printf("Found a Hit on plane with angle: %f \n", angle);
		}

		return h;
	}

	Vec3f Plane::getShadowColor(Ray incomingray, Vec3f lightpos, Vec3f lightid, Vec3f lightis,  Hit hitPoint, JPGLoader *loader)
	{
		Vec3f diff = material->getDiffuseColor(incomingray, lightpos,lightid, hitPoint);
		if (!material->tPath._Equal(""))
		{
			int w = material->tWidth;
			int h = material->tHeight;
			Vec3f A = v0; // Using point v0 as the default corner in texture, which is (0,0)
			Vec3f B = v1;
			Vec3f C = v2;
			Vec3f D = v3;

			Vec3f P = hitPoint.point;
			Vec3f AB = B - A; // AB as u axis
			Vec3f AD = D - A; // AD as v axis
			Vec3f AP = P - A;
			// since p is on plane ABCD, P = A + u * AB + V * AD
			float u = AP.dotProduct(AB) / AB.length();
			float v = AP.dotProduct(AD) / AD.length();

			u = u * w;
			int u_valid = u;
			while (u_valid > w)	u_valid -= w;
			while (u_valid < 0) u_valid += w;
			v = v * h;
			int v_valid = v;
			while (v_valid > h) v_valid -= h;
			while (v_valid < 0) v_valid += h;

			Vec3f pixelcolor = loader->getValue(material->tPath, w, h, u_valid, v_valid);
			diff = pixelcolor * (diff.length() / material->diffusecolor.length());
		}
		Vec3f spec = material->getSpecularColor(incomingray, lightpos,lightis, hitPoint);
		Vec3f res = diff + spec;
		res.x = std::max(res.x, 0.0f);
		res.y = std::max(res.y, 0.0f);
		res.z = std::max(res.z, 0.0f);
		return res;
	}

	Vec3f Plane::getDistributedDiffColor(Ray incomingray, Vec3f raydir,  Vec3f lightcolor, Hit hitPoint, JPGLoader *loader){
		Vec3f diff = material->getDistributedDiff(incomingray, raydir,lightcolor, hitPoint);
		if (!material->tPath._Equal(""))
		{
			int w = material->tWidth;
			int h = material->tHeight;
			Vec3f A = v0; // Using point v0 as the default corner in texture, which is (0,0)
			Vec3f B = v1;
			Vec3f C = v2;
			Vec3f D = v3;

			Vec3f P = hitPoint.point;
			Vec3f AB = B - A; // AB as u axis
			Vec3f AD = D - A; // AD as v axis
			Vec3f AP = P - A;
			// since p is on plane ABCD, P = A + u * AB + V * AD
			float u = AP.dotProduct(AB) / AB.length();
			float v = AP.dotProduct(AD) / AD.length();

			u = u * w;
			int u_valid = u;
			while (u_valid > w)	u_valid -= w;
			while (u_valid < 0) u_valid += w;
			v = v * h;
			int v_valid = v;
			while (v_valid > h) v_valid -= h;
			while (v_valid < 0) v_valid += h;

			Vec3f pixelcolor = loader->getValue(material->tPath, w, h, u_valid, v_valid);
			diff = pixelcolor * (diff.length() / material->diffusecolor.length());
		}
		Vec3f res = diff;
		res.x = std::max(res.x, 0.0f);
		res.y = std::max(res.y, 0.0f);
		res.z = std::max(res.z, 0.0f);
		return res;
	}

	Vec3f Plane::getDistributedSpecColor(Ray incomingray, Vec3f raydir,  Vec3f lightcolor, Hit hitPoint, JPGLoader *loader){
		Vec3f res = material->getDistributedSpec(incomingray,raydir,lightcolor,hitPoint);
		res.x = std::max(res.x, 0.0f);
		res.y = std::max(res.y, 0.0f);
		res.z = std::max(res.z, 0.0f);
		return res;
	}

	boundingBox Plane::getBound()
	{
		float xMax, xMin, yMax, yMin, zMax, zMin;
		xMax = std::max(v0.x, std::max(v1.x, std::max(v2.x, v3.x)));
		xMin = std::min(v0.x, std::min(v1.x, std::min(v2.x, v3.x)));
		yMax = std::max(v0.y, std::max(v1.y, std::max(v2.y, v3.y)));
		yMin = std::min(v0.y, std::min(v1.y, std::min(v2.y, v3.y)));
		zMax = std::max(v0.z, std::max(v1.z, std::max(v2.z, v3.z)));
		zMin = std::min(v0.z, std::min(v1.z, std::min(v2.z, v3.z)));

		return boundingBox(xMax, xMin, yMax, yMin, zMax, zMin);
	}

	Vec3f Plane::getRandomPoint(){
		Vec3f p = Vec3f();
		Vec3f A = v0; // Using point v0 as the default corner in texture, which is (0,0)
		Vec3f B = v1;
		Vec3f D = v3;
		Vec3f AB = B - A;
		Vec3f AD = D - A;
		Vec3f norm = AD.crossProduct(AB);

		bool inside = false;

		// Assuming a rectangle or parallelogram
		float b = (std::rand() * 1.0f) / (1.0f * RAND_MAX);
		float d = (std::rand() * 1.0f) / (1.0f * RAND_MAX);
		p = A + AB * b + AD * d;

		return p;
	}

	Plane::~Plane()
	{
		delete this->material;
	}

} // namespace rt
