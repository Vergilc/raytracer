/*
 * LightSource.cpp
 *
 */
#include "LightSource.h"
#include "lights/PointLight.h"
#include "lights/AreaLight.h"
#include "rapidjson/document.h"

namespace rt
{

    LightSource::~LightSource(){};

    LightSource *LightSource::createLightSource(Value &light)
    {

        // check if cameratype is defined

        std::string lighttype;
        if (!light.HasMember("type"))
        {
            std::cerr << "Lightsource type not specified, using default 'pointlight'\n" << std::endl;
            lighttype = "pointlight";
        }
        else
        {
            lighttype = light["type"].GetString();
        }

        Vec3f position = Vec3f(light["position"].GetArray()[0].GetFloat(), light["position"].GetArray()[1].GetFloat(), light["position"].GetArray()[2].GetFloat());
        Vec3f is = Vec3f(light["is"].GetArray()[0].GetFloat(), light["is"].GetArray()[1].GetFloat(), light["is"].GetArray()[2].GetFloat())*(1/255.f);
        Vec3f id = Vec3f(light["id"].GetArray()[0].GetFloat(), light["id"].GetArray()[1].GetFloat(), light["id"].GetArray()[2].GetFloat())*(1/255.f);
        printf("haha");
        if (lighttype.compare("pointlight") == 0)
        {
            return new PointLight(position, is, id);
        }
        else if (lighttype.compare("arealight") == 0)
        {
            if (light.HasMember("shapeid"))
            {
                std::string shapeid = light["shapeid"].GetString();
                return new AreaLight(position, is, id, shapeid);
            }
            else
            {
                std::cerr << "Shape id for arealight not specified, using default 'pointlight'\n" << std::endl;
                return new PointLight(position, is, id);
            }      
        }

        return 0;
    }
} // namespace rt
