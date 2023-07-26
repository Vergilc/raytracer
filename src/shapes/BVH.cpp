/*
 * BVH.cpp
 *
 *
 */
#include "BVH.h"


namespace rt{

    BVH::BVH():boundingbox(boundingBox(0,0,0,0,0,0)){
		this->id = "newBVH";
	}

	BVH::BVH(std::string id, boundingBox box):boundingbox(box){
		this->id = id;
	}

    BVH* BVH::createBVH(std::vector<Shape*> shapes){
		boundingBox box = shapes[0]->getBound();
		BVH* bvh = new BVH("newBVH", box);
		if (shapes.size() == 1){
			bvh->lbvh = nullptr;
			bvh->rbvh = nullptr;
			bvh->shape = shapes[0];
			return bvh;
		}

		for (int i = 0; i < shapes.size(); i++){
			box = mergeBox(box, shapes[i]->getBound());
		}
		bvh->boundingbox = box;

		boundingBox lbox = box;
		float x_dif = box.xMax - box.xMin;
		float y_dif = box.yMax - box.yMin;
		float z_dif = box.zMax - box.zMin;
		if (x_dif > y_dif){
			if (x_dif > z_dif){
				// split x
				lbox.xMax = (lbox.xMin + lbox.xMax) / 2;
			}
			else{
				//split z
				lbox.zMax = (lbox.zMin + lbox.zMax) / 2;
			}
		} else if (y_dif < z_dif){
			//split z
			lbox.zMax = (lbox.zMin + lbox.zMax) / 2;
		} else{
			//split y
			lbox.yMax = (lbox.yMin + lbox.yMax) / 2;
		}
		std::vector<Shape*> lshapes = {};
		std::vector<Shape*> rshapes = {};
		for (int i = 0; i < shapes.size(); i++){
			boundingBox newbox = shapes[i]->getBound();
			if (contains(lbox, newbox)){
				lshapes.push_back(shapes[i]);
			} else{
				rshapes.push_back(shapes[i]);
			}
		}
		if (lshapes.size()==0){
			lshapes.push_back(rshapes[0]);
			rshapes = {rshapes.begin() + 1, rshapes.end()};
		}
		if (rshapes.size()==0){
			rshapes.push_back(lshapes[0]);
			lshapes = {lshapes.begin() + 1, lshapes.end()};
		}
		bvh->lbvh = BVH::createBVH(lshapes);
		bvh->rbvh = BVH::createBVH(rshapes);
		bvh->shape = nullptr;

        return bvh;
    } 

	boundingBox BVH::getBound(){
		return boundingbox;
	}


	/**
	 * Computes whether a ray hit the specific instance of a sphere shape and returns the hit data
	 *
	 * @param ray cast ray to check for intersection with shape
	 *
	 * @return hit struct containing intersection information
	 *
	 */
	Hit BVH::intersect(Ray ray){

		Hit h = Hit();
		std::cout<< "Warning: BVH can not hit a ray!\n"; 
		return h;

	}

	std::pair<Hit, Shape*> BVH::findHit(Ray ray){
		if (this->shape != nullptr){
			Hit hit = this->shape->intersect(ray);
			std::pair<Hit, Shape*> res = std::make_pair(hit, this->shape);
			return res;
		}
		//printf("Not end node, check l and r\n");
		float dist = -1.f;
		Hit hit = Hit();
		Shape *hitedShape = nullptr;
		if (this->lbvh == nullptr){
			std::cout<<"Warning: BVH empty left node and no shape";
		} else if (boxrayintersect(lbvh->boundingbox,ray)){
			std::pair<Hit, Shape*> lresult = lbvh->findHit(ray);
			Hit lhit = lresult.first;
			if (lhit.isHit){
				if (lhit.distance < dist || dist < 0){
					hit = lhit;
					hitedShape = lresult.second;
					dist = lhit.distance;
				}
			}
		} 
		if (this->rbvh == nullptr){
			std::cout<<"Warning: BVH empty right node and no shape";
		} else if (boxrayintersect(rbvh->boundingbox,ray)){
			std::pair<Hit, Shape*> rresult = rbvh->findHit(ray);
			Hit rhit = rresult.first;
			if (rhit.isHit){
				if (rhit.distance < dist || dist < 0){
					hit = rhit;
					hitedShape = rresult.second;
					dist = rhit.distance;
				}
			}
		} 
		return std::make_pair(hit, hitedShape);
	}

	Vec3f BVH::getShadowColor(Ray incomingray, Vec3f lightpos, Vec3f lightid, Vec3f lightis,  Hit hitPoint, JPGLoader *loader)
    {
        if (this->shape != nullptr){
			return this->shape->getShadowColor(incomingray, lightpos,lightid,lightis, hitPoint,loader);
		}
		printf("Warning: getting shadow color form BVH!\n");
 		return Vec3f(0,0,0);
    }

	Vec3f BVH::getRandomPoint(){
		return Vec3f();
	}

	BVH::~BVH(){
		delete this->material;
		delete this->lbvh;
		delete this->rbvh;
	}
} //namespace rt


