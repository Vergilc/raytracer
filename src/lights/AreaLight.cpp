/*
 * AreaLight.cpp
 *
 *
 */
#include "AreaLight.h"

namespace rt
{

    AreaLight::AreaLight(Vec3f position, Vec3f is, Vec3f id, std::string shapeid) : LightSource(position, is, id, shapeid)
    {
        printf("Generating new Arealight!\n");
        // to fill
    }

    AreaLight* convertToAL(LightSource *light){
        return new AreaLight(light->position,light->is, light->id, light->shapeid);
    }
    

} // namespace rt
