/*
 * Triangle.cpp
 *
 *
 */
#include "Triangle.h"

namespace rt
{
	Triangle::Triangle(std::string id, Material *material, Vec3f v0, Vec3f v1, Vec3f v2)
	{
		this->id = id;
		this->material = material;
		this->v0 = v0;
		this->v1 = v1;
		this->v2 = v2;
	}

	/**
	 * Computes whether a ray hit the specific instance of a sphere shape and returns the hit data
	 *
	 * @param ray cast ray to check for intersection with shape
	 *
	 * @return hit struct containing intersection information
	 *
	 */
	Hit Triangle::intersect(Ray ray)
	{
		Hit h = Hit();
		//-----------to be implemented -------------

		Vec3f origin = ray.origin;
		Vec3f dir = ray.direction;

		RayType raytype = ray.raytype;
		// the while plane of ACB: n * p + d = 0
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

		Vec3f p = origin + dir * t; // point of intersection

		// check if inside Plane ABC :
		// r1 = n * (AP X AB)
		float r1 = norm.dotProduct((p - v0).crossProduct(v1 - v0));
		float r2 = norm.dotProduct((p - v1).crossProduct(v2 - v1));
		float r3 = norm.dotProduct((p - v2).crossProduct(v0 - v2));

		if ((r1 * r2 > 0) && (r2 * r3 > 0) && (r1 * r3 > 0) && t > 0)
		{
			// intersection with plane at p confirmed

			float distance = (p - origin).length(); // distance from ray start point to the first interaction point
			dir = dir.normalize();
			norm = norm.normalize();
			// angle = arccos(a * b) / (a.len * b.len)
			if (dir.dotProduct(norm) > 0){
				norm = -norm;
			}
			float angle = acos((-dir).dotProduct(norm));
			Vec3f projection = norm * (norm.dotProduct(dir));
			Vec3f reflectiondir = dir - 2 * projection;
			h = Hit(p, distance, angle, norm, reflectiondir);
			//printf("Found a Hit on triangle with angle: %f \n", angle);
		}

		return h;
	}

	Vec3f Triangle::getShadowColor(Ray incomingray, Vec3f lightpos, Vec3f lightid, Vec3f lightis,  Hit hitPoint , JPGLoader *loader)
    {
        Vec3f diff = material->getDiffuseColor(incomingray, lightpos,lightid, hitPoint);
		if (!material->tPath._Equal("")){
			Vec3f A = v0;	// Using point v0 as the default corner in texture, which is (0,0)
			Vec3f B = v1;
			Vec3f C = v2;
			int w = material->tWidth;
			int h = material->tHeight;
			// std::cout<< w << "," << h <<"\n";
			
			Vec3f P = hitPoint.point;
			Vec3f AP = P - A;
            Vec3f AB = B - A;				//AB as u axis
			Vec3f AC = C - A;
			float ac_len = AC.length(); 
			Vec3f n = AB.crossProduct(AC);
			AC = n.crossProduct(AB).normalize() * ac_len;		//AC as v axis
            // since p is on plane ABC, P = A + u * AB + v * AD

			float u = AP.dotProduct(AB) / AB.length();
			float v = AP.dotProduct(AC) / AC.length();

            u = u * w;
            int u_valid = u;
			if (u_valid > w) u_valid = u_valid % w;
			if (u_valid < 0) u_valid = u_valid % w + w;
			v = v * h;
			int v_valid = v;
			if (v_valid > h) v_valid = v_valid % h;
			if (v_valid < 0) v_valid = v_valid % h + h;
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

	Vec3f Triangle::getDistributedDiffColor(Ray incomingray, Vec3f raydir,  Vec3f lightcolor, Hit hitPoint, JPGLoader *loader)
	{
        Vec3f diff = material->getDistributedDiff(incomingray, raydir,lightcolor, hitPoint);
		if (!material->tPath._Equal("")){
			int w = material->tWidth;
			int h = material->tHeight;
			Vec3f A = v0;	// Using point v0 as the default corner in texture, which is (0,0)
			Vec3f B = v1;
			Vec3f C = v2;

			Vec3f P = hitPoint.point;
            Vec3f AB = B - A;				//AB as u axis
			Vec3f AC = C - A;
			Vec3f AP = P - A;
			float ac_len = AC.length(); 
			Vec3f n = AB.crossProduct(AC);
			AC = n.crossProduct(AB).normalize() * ac_len;		//AC as v axis
			// since p is on plane ABC, P = A + u * AB + V * AD
			float u = AP.dotProduct(AB) / AB.length();
			float v = AP.dotProduct(AC) / AC.length();
			u = u * w;
			int u_valid = u;
			if (u_valid > w) u_valid = u_valid % w;
			if (u_valid < 0) u_valid = u_valid % w + w;
			v = v * h;
			int v_valid = v;
			if (v_valid > h) v_valid = v_valid % h;
			if (v_valid < 0) v_valid = v_valid % h + h;
			Vec3f pixelcolor = loader->getValue(material->tPath, w, h,u_valid, v_valid);
			diff = pixelcolor * (diff.length() / material->diffusecolor.length());
		}
		diff.x = std::max(diff.x, 0.0f);
		diff.y = std::max(diff.y, 0.0f);
		diff.z = std::max(diff.z, 0.0f);
		return diff;
    }

	Vec3f Triangle::getDistributedSpecColor(Ray incomingray, Vec3f raydir,  Vec3f lightcolor, Hit hitPoint, JPGLoader *loader)
	{
        Vec3f res = material->getDistributedSpec(incomingray, raydir, lightcolor, hitPoint);
		res.x = std::max(res.x, 0.0f);
		res.y = std::max(res.y, 0.0f);
		res.z = std::max(res.z, 0.0f);
		return res;
    }

	boundingBox Triangle::getBound(){
		float xMax, xMin, yMax, yMin, zMax, zMin;
		xMax = std::max(v0.x,std::max(v1.x,v2.x));
		xMin = std::min(v0.x,std::min(v1.x,v2.x));
		yMax = std::max(v0.y,std::max(v1.y,v2.y));
		yMin = std::min(v0.y,std::min(v1.y,v2.y));
		zMax = std::max(v0.z,std::max(v1.z,v2.z));
		zMin = std::min(v0.z,std::min(v1.z,v2.z));

		return boundingBox(xMax, xMin, yMax, yMin, zMax, zMin);
	}

	Vec3f Triangle::getRandomPoint(){
		Vec3f AB = v1 - v0;
		Vec3f AC = v2 - v0;
		Vec3f norm = AC.crossProduct(AB);
		Vec3f p;
		float x = std::rand() * 1.0f / RAND_MAX;
		float y = std::rand() * 1.0f / RAND_MAX;
		p = v0 + AB * x + AC * y;
		float r1 = norm.dotProduct((p - v0).crossProduct(v1 - v0));
		float r2 = norm.dotProduct((p - v1).crossProduct(v2 - v1));
		float r3 = norm.dotProduct((p - v2).crossProduct(v0 - v2));
		if (!(r1 * r2 > 0) || !(r2 * r3 > 0) || !(r1 * r3 > 0)) p = v0 + AB * (1-x) + AC * (1-y);
		return p;
	}

	Triangle::~Triangle(){
		delete this->material;
	}

} // namespace rt
