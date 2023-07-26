/*
 * Material.h
 *
 */

#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "math/geometry.h"
#include "rapidjson/document.h"
#include "core/LightSource.h"
#include "core/RayHitStructs.h"

using namespace rapidjson;

namespace rt
{

	class Material
	{
	public:
		Material(){};
		Material(float ks_in, float kd_in, float kr_in,float kt_in, float eta_in, int specularexponent_in, Vec3f diffusecolor_in, std::string tPath_in, int tWidth_in, int tHeight_in) : ks(ks_in), kd(kd_in), kr(kr_in),kt(kt_in),eta(eta_in),specularexponent(specularexponent_in), tPath(tPath_in), diffusecolor(diffusecolor_in), tWidth(tWidth_in), tHeight(tHeight_in){};

		virtual ~Material();

		//
		// factory function : returns camera instance dynamically based on camera specificationsFF
		//
		static Material *createMaterial(Value &materialSpecs);

		bool isReflective()
		{
			return (kr > 0.0001f);
		};
		bool istransparent()
		{
			return (kt > 0.0001f);
		};

		virtual Vec3f getShadowColor(Ray incomingray, Vec3f lightpos, Vec3f lightid, Vec3f lightis, Hit hitPoint) = 0;
		virtual Vec3f getDiffuseColor(Ray incomingray, Vec3f lightpos, Vec3f lightid, Hit hitPoint) = 0;
        virtual Vec3f getSpecularColor(Ray incomingray, Vec3f lightpos, Vec3f lightis, Hit hitPoint) = 0;
        virtual Vec3f getDistributedDiff(Ray incomingray, Vec3f raydir,  Vec3f lightcolor, Hit hitPoint) = 0;
        virtual Vec3f getDistributedSpec(Ray incomingray, Vec3f raydir,  Vec3f lightcolor, Hit hitPoint) = 0;


		float ks = 0.0f;
		float kd = 0.0f;
		float kr = 0.0f;
		float kt = 0.0f;
		float eta = 1.0f;
		int specularexponent = 3;
		Vec3f diffusecolor = Vec3f(1.0f, 1.0f, 1.0f);
		std::string tPath;
		int tWidth = 1;
		int tHeight = 1;
	};

} // namespace rt

#endif /* MATERIAL_H_ */
