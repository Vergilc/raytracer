/*
 * BlinnPhong.cpp
 *
 *
 */

#include "BlinnPhong.h"

namespace rt
{
    BlinnPhong::BlinnPhong(float ks_in, float kd_in, float kr_in,float kt_in, float eta_in, int specularexponent_in, Vec3f diffusecolor_in, std::string tPath_in, int tWidth_in, int tHeight_in): Material(ks_in,kd_in,kr_in,kt_in,eta_in,specularexponent_in,diffusecolor_in,tPath_in,tWidth_in,tHeight_in)
    {
        this->ks = ks_in;
        this->kd = kd_in;
        this->kr = kr_in;
        this->kt = kt_in;
        this->eta = eta_in;
        this->specularexponent = specularexponent_in,
        this->tPath = tPath_in;
        this->diffusecolor = diffusecolor_in;
        this->tWidth = tWidth_in;
        this->tHeight = tHeight_in;
    }

    Vec3f BlinnPhong::getShadowColor(Ray incomingray, Vec3f lightpos, Vec3f lightid, Vec3f lightis, Hit hitPoint)
    {
        Vec3f diffcolor = getDiffuseColor(incomingray, lightpos,lightid, hitPoint);
        Vec3f speccolor = getSpecularColor(incomingray, lightpos,lightis, hitPoint);
        return (speccolor + diffcolor);
    }

    Vec3f BlinnPhong::getDiffuseColor(Ray incomingray, Vec3f lightpos, Vec3f lightid, Hit hitPoint){
        Vec3f i = (lightpos - hitPoint.point ).normalize();
        Vec3f norm = hitPoint.norm;
        float diff_light = (norm.normalize().dotProduct(i));
        Vec3f diffcolor = diffusecolor * diff_light * kd * lightid;
        float dist = (lightpos - hitPoint.point).length();
        return diffcolor * (1.f/ pow(dist/3,2.0f));
    }

    Vec3f BlinnPhong::getSpecularColor(Ray incomingray,Vec3f lightpos,  Vec3f lightis, Hit hitPoint){
        Vec3f v = -incomingray.direction;
        v.normalize();
        Vec3f i = (lightpos - hitPoint.point).normalize();
        Vec3f h = (v + i).normalize();
        Vec3f norm = hitPoint.norm;
        float spec = h.dotProduct(norm.normalize());
        float exp = specularexponent * 1.0f;
        Vec3f speccolor = lightis * pow(spec, exp) * ks;
        float dist = (lightpos- hitPoint.point).length();
        return speccolor * (1.f/ pow(dist/3,2.0f));
    }

    Vec3f BlinnPhong::getDistributedDiff(Ray incomingray, Vec3f raydir,  Vec3f lightcolor, Hit hitPoint){
        Vec3f v = -incomingray.direction;
        v.normalize();
        Vec3f i = (raydir).normalize();
        Vec3f h = (v + i).normalize();
        Vec3f norm = hitPoint.norm;
        float diff_light = (norm.normalize().dotProduct(i));
        Vec3f diffcolor = lightcolor * diffusecolor * diff_light * kd ;
        return diffcolor;
    }

    Vec3f BlinnPhong::getDistributedSpec(Ray incomingray, Vec3f raydir,  Vec3f lightcolor, Hit hitPoint){
        Vec3f v = -incomingray.direction;
        v.normalize();
        Vec3f i = (raydir).normalize();
        Vec3f h = (v + i).normalize();
        Vec3f norm = hitPoint.norm;
        float spec = h.dotProduct(norm.normalize());
        float spec_light = pow(spec, specularexponent * 1.0f);
        Vec3f speccolor = lightcolor * spec_light * ks;
        return speccolor;
    }

} // namespace rt
