/*
 * PointLight.h
 *
 *
 */

#ifndef POINTLIGHT_H_
#define POINTLIGHT_H_

#include "core/LightSource.h"
#include "math/geometry.h"

namespace rt
{

	class PointLight : public LightSource
	{
	public:
		//
		// Constructors
		//
		PointLight(){};
		PointLight(Vec3f position, Vec3f is, Vec3f id);

		//
		// Destructor
		//
		~PointLight(){};
	};

} // namespace rt

#endif /* POINTLIGHT_H_ */
