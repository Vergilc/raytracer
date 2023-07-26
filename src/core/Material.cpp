/*
 * Material.cpp
 *
 */
#include "Material.h"
#include "materials/BlinnPhong.h"

namespace rt
{

    Material::~Material(){};

    Material *Material::createMaterial(Value &materialSpec)
    {

        float ks = 0.0f;
        float kd = 0.0f;
        float kr = 0.0f;
        float kt = 0.0f;
        float eta = 1.0f;
        int specularexponent = 3;
        Vec3f diffusecolor = Vec3f(0.0f, 0.0f, 0.0f);
        std::string tPath = "";
        int tWidth = 1;
        int tHeight = 1;

        if (materialSpec.HasMember("ks"))
        {
            ks = materialSpec["ks"].GetFloat();
        }
        if (materialSpec.HasMember("kd"))
        {
            kd = materialSpec["kd"].GetFloat();
        }
        if (materialSpec.HasMember("kr"))
        {
            kr = materialSpec["kr"].GetFloat();
        }
        if (materialSpec.HasMember("kt"))
        {
            kt = materialSpec["kt"].GetFloat();
        }
        if (materialSpec.HasMember("eta"))
        {
            eta = materialSpec["eta"].GetFloat();
        }
        if (materialSpec.HasMember("specularexponent"))
        {
            specularexponent = materialSpec["specularexponent"].GetInt();
        }
        if (materialSpec.HasMember("diffusecolor"))
        {
            diffusecolor = Vec3f(materialSpec["diffusecolor"].GetArray()[0].GetFloat(), materialSpec["diffusecolor"].GetArray()[1].GetFloat(), materialSpec["diffusecolor"].GetArray()[2].GetFloat());
        }
        if (materialSpec.HasMember("tPath"))
        {
            tPath = materialSpec["tPath"].GetString();
        }
        if (materialSpec.HasMember("tWidth"))
        {
            tWidth = materialSpec["tWidth"].GetInt();
        }
        if (materialSpec.HasMember("tHeight"))
        {
            tHeight = materialSpec["tHeight"].GetInt();
        }
        // using BlinnPhong as default
        return new BlinnPhong(ks, kd, kr, kt, eta, specularexponent, diffusecolor, tPath, tWidth, tHeight);
    }

} // namespace rt
