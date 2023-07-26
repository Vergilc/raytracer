/*
 * Scene.cpp
 *
 */
#include "Scene.h"
#include "shapes/BVH.h"
#include "lights/PointLight.h"

namespace rt{

/**
 * Parses json scene object to generate scene to render
 *
 * @param scenespecs the json scene specificatioon
 */
void Scene::createScene(Value& scenespecs){

	//----------parse json object to populate scene-----------
	
	// parse background color
	if (!scenespecs.HasMember("backgroundcolor")){
		std::cerr<<"backgroundcolor not specified, using default black"<<std::endl;
		bgcolor = Vec3f(0,0,0);
	} else{
		bgcolor = Vec3f(scenespecs["backgroundcolor"].GetArray()[0].GetFloat(),scenespecs["backgroundcolor"].GetArray()[1].GetFloat(),scenespecs["backgroundcolor"].GetArray()[2].GetFloat());
		std::cout<< "Loading background color: "<<bgcolor << std::endl;
	}

	// parse light sources
	if (!scenespecs.HasMember("lightsources")){
		std::cerr<<"Warning: no light sources specified, may result in a pure black result!"<<std::endl;
	} else{
		GenericArray<false, rapidjson::Value> lights = scenespecs["lightsources"].GetArray();
		for (int i = 0; i < lights.Size(); i ++){
			Value light = lights[i].GetObject();
			LightSource* lightsource=LightSource::createLightSource(light);
			lightSources.push_back(lightsource);

		}
		printf("Scene has %d light sources.\n", lightSources.size());
	}

	std::string acc;
	if (!scenespecs.HasMember("accelerator")){
		std::cout<<"Accelerator not specified, using BVH as default"<<std::endl;
		acc = "bvh";
	} else{
		acc = scenespecs["accelerator"].GetString();
	}

	// parse shapes
	if (!scenespecs.HasMember("shapes")){
		std::cerr<<"Warning: no shapes specified!"<<std::endl;
		bvh = nullptr;
	} else{
		auto allshapes = scenespecs["shapes"].GetArray();
		for (int j = 0; j < allshapes.Size(); j ++){
			Value shapeSpec = allshapes[j].GetObject();
			Shape* shape=Shape::createShape(shapeSpec);
			shapes.push_back(shape);
		}
		if (shapes.size() == 0) {
			printf("Warning: no shapes found in shapespec!\n");
			bvh = nullptr;
		}
		else if (acc.compare("bvh") == 0) {
			BVH* newbvh = BVH::createBVH(shapes);
			printf("BVH generated\n");	
			bvh = newbvh;
		} 
		else if(acc.compare("none")==0) bvh = nullptr;
		else {
			std::cerr<<"Unknown accelerator specified: "<< acc <<std::endl;
			std::cerr<<"Using bvh as default."<< acc <<std::endl;
			BVH* newbvh = BVH::createBVH(shapes);
			printf("BVH generated\n");	
			bvh = newbvh;
		}
	}

	// update shape information for arealights
	for (int i = 0; i < lightSources.size(); i++){
		if (!lightSources[i]->shapeid._Equal("")){
			bool found = false;
			std::string shapeid = lightSources[i]->shapeid;
			for (auto shape: shapes){
				if (shape->getId()._Equal(shapeid)){
					if (found) {
						std::cout << "Warning: multiple shape id matches the arealight shapeid! Using the first found." << std::endl;
					} else{
						shape->islight();
						found = true;
					}
				}
			}
			if (!found){
				std::cerr << "Shape id for area light not found in shapes! This arealight is converted to a point light!" << std::endl;
				LightSource* oldAreaLight = lightSources[i];
				LightSource* newPointLight = new PointLight(oldAreaLight->position, oldAreaLight->is, oldAreaLight->id);
				lightSources[i] = newPointLight;
				delete oldAreaLight;
			}
		}
	}
}

std::pair<Hit,Shape*> Scene::findHit(Ray r){
	Hit h;
	Shape* shapehit;

	if (this->bvh != nullptr){
		return bvh->findHit(r);
	}

	//printf("Current scene has %d shapes\n", shapes.size());
	for (int i = 0; i < this->shapes.size(); i++){
		if (shapes[i]->checkislight()) continue;	// TODO: currently ignore arealight shape
		Shape* nextShape = shapes[i];
		Hit nextHit = nextShape->intersect(r);
		if (nextHit.isHit){
			if (!h.isHit){
				h = nextHit;
				shapehit = nextShape;
			}
			else if (nextHit.distance < h.distance){
				h = nextHit;
				shapehit = nextShape;
			}
		}
	}
	std::pair<Hit, Shape*> res = std::make_pair(h,shapehit);
	return res;
}

Shape* Scene::findShape(std::string shapeid){
	for (int i = 0; i < this->shapes.size(); i++){
		if (shapes[i]->getId()._Equal(shapeid)){
			return shapes[i];
		}
	}
	std::cerr<< "Error: given shapeid not found in shapes!" << std::endl;
	return nullptr;
}

Scene::~Scene(){
	for (auto light : lightSources){
		delete light;
	}
	for (auto shape : shapes){
		delete shape;
	}
	delete bvh;
}

} //namespace rt
