/*
 * LightSource.h
 *
 */

#ifndef LIGHTSOURCE_H_
#define LIGHTSOURCE_H_

#include "math/geometry.h"
#include "rapidjson/document.h"

using namespace rapidjson;

namespace rt
{

	class LightSource
	{
	public:
		LightSource(){};
		LightSource(Vec3f position, Vec3f is, Vec3f id, std::string sid) : position(position), is(is), id(id), shapeid(sid){};
		//
		// Destructor (must be overriden in subclass)
		//
		virtual ~LightSource();

		//
		// factory function : returns camera instance dynamically based on camera specifications
		//
		static LightSource *createLightSource(Value &lightSpecs);

		Vec3f position;
		Vec3f is; // spectrum
		Vec3f id; // density
		std::string shapeid;

	private:
	};

} // namespace rt

#endif /* LIGHTSOURCE_H_ */
