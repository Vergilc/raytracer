/*
 * BlinnPhong.h
 *
 *
 */

#ifndef BLINNPHONG_H_
#define BLINNPHONG_H_

#include "core/Material.h"

namespace rt
{

    class BlinnPhong : public Material
    {
    public:
        BlinnPhong(){};
        BlinnPhong(float ks_in, float kd_in, float kr_in,float kt_in, float eta_in, int specularexponent_in, Vec3f diffusecolor_in, std::string tPath_in, int tWidth_in, int tHeight_in);

        ~BlinnPhong(){};

        Vec3f getShadowColor(Ray incomingray, Vec3f lightpos, Vec3f lightid, Vec3f lightis, Hit hitPoint);
        Vec3f getDiffuseColor(Ray incomingray, Vec3f lightpos, Vec3f lightid, Hit hitPoint);
        Vec3f getSpecularColor(Ray incomingray, Vec3f lightpos, Vec3f lightis, Hit hitPoint);
        Vec3f BlinnPhong::getDistributedDiff(Ray incomingray, Vec3f raydir,  Vec3f lightcolor, Hit hitPoint);
        Vec3f BlinnPhong::getDistributedSpec(Ray incomingray, Vec3f raydir,  Vec3f lightcolor, Hit hitPoint);

    };

} // namespace rt

#endif /* BLINNPHONG_H_ */
