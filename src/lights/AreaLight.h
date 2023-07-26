/*
 * AreaLight.h
 *
 *
 */

#ifndef AREALIGHT_H_
#define AREALIGHT_H_

#include "core/LightSource.h"
#include "core/Shape.h"
#include "math/geometry.h"

namespace rt
{

	class AreaLight : public LightSource
	{
	public:
		//
		// Constructors
		//
		AreaLight(){};
		AreaLight(Vec3f position, Vec3f is, Vec3f id, std::string shapeid);
		//
		// Destructor
		//
		~AreaLight(){};
	};

	AreaLight* convertToAL(LightSource *light);

} // namespace rt

#endif /* AREALIGHT_H_ */
