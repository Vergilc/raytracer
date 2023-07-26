/*
 * TriMesh.h
 *
 *
 */

#ifndef TRIMESH_H_
#define TRIMESH_H_

#include "core/Shape.h"

namespace rt
{

	class TriMesh : public Shape
	{
	public:
		//
		// Constructors
		//
		TriMesh();
		TriMesh(std::string id, Material *material, std::vector<Vec3f> vs, std::vector<Vec3i> fs);
		TriMesh(std::string id, Material *material, std::string filename);

		~TriMesh();

		Hit intersect(Ray ray);
		Vec3f getShadowColor(Ray incomingray, Vec3f lightpos, Vec3f lightid, Vec3f lightis,  Hit hitPoint, JPGLoader *loader);
		Vec3f getDistributedDiffColor(Ray incomingray, Vec3f raydir,  Vec3f lightcolor, Hit hitPoint, JPGLoader *loader);
		Vec3f getDistributedSpecColor(Ray incomingray, Vec3f raydir,  Vec3f lightcolor, Hit hitPoint, JPGLoader *loader);
		boundingBox getBound();

		Vec3f getRandomPoint();
		
	private:
		std::vector<Vec3f> vertices;
		std::vector<Vec3i> faces;
	};

} // namespace rt

#endif /* TRIMESH_H_ */
