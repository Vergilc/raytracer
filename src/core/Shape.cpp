/*
 * Shape.cpp
 *
 */
#include "Shape.h"
#include "shapes/Sphere.h"
#include "shapes/TriMesh.h"
#include "shapes/Triangle.h"
#include "shapes/BVH.h"
#include "shapes/Plane.h"

namespace rt
{
    Shape::~Shape()
    {
        delete this->material;
    };

    Shape *Shape::createShape(Value &shapeSpec)
    {

        // check if cameratype is defined

        std::string shapetype;
        std::string newid;

        Material *newMaterial;

        if (!shapeSpec.HasMember("id"))
        {
            std::cerr << "Warning:shape id not specified" << std::endl;
            newid = "NewShape";
        }
        else
        {
            newid = shapeSpec["id"].GetString();
        }

        if (!shapeSpec.HasMember("type"))
        {
            std::cerr << "Error: shape type not specified" << std::endl;
            exit(1);
        }
        else
        {
            shapetype = shapeSpec["type"].GetString();
        }

        if (!shapeSpec.HasMember("material"))
        {
            std::cerr << "Warning:shape material not specified, using default 'BlinnPhong' instead" << std::endl;
            newMaterial = new BlinnPhong();
            // material = none;
        }
        else
        {
            newMaterial = Material::createMaterial(shapeSpec["material"]);
        }

        if (shapetype.compare("sphere") == 0)
        {
            Vec3f center = Vec3f(shapeSpec["center"].GetArray()[0].GetFloat(), shapeSpec["center"].GetArray()[1].GetFloat(), shapeSpec["center"].GetArray()[2].GetFloat());
            float radius = shapeSpec["radius"].GetFloat();
            return new Sphere(newid, newMaterial, center, radius);
        }
        else if (shapetype.compare("plane") == 0)
        {
            Vec3f v0(shapeSpec["v0"].GetArray()[0].GetFloat(), shapeSpec["v0"].GetArray()[1].GetFloat(), shapeSpec["v0"].GetArray()[2].GetFloat());
            Vec3f v1(shapeSpec["v1"].GetArray()[0].GetFloat(), shapeSpec["v1"].GetArray()[1].GetFloat(), shapeSpec["v1"].GetArray()[2].GetFloat());
            Vec3f v2(shapeSpec["v2"].GetArray()[0].GetFloat(), shapeSpec["v2"].GetArray()[1].GetFloat(), shapeSpec["v2"].GetArray()[2].GetFloat());
            Vec3f v3(shapeSpec["v3"].GetArray()[0].GetFloat(), shapeSpec["v3"].GetArray()[1].GetFloat(), shapeSpec["v3"].GetArray()[2].GetFloat());
            return new Plane(newid, newMaterial, v0, v1, v2, v3);
        }
        else if (shapetype.compare("triangle") == 0)
        {
            Vec3f v0(shapeSpec["v0"].GetArray()[0].GetFloat(), shapeSpec["v0"].GetArray()[1].GetFloat(), shapeSpec["v0"].GetArray()[2].GetFloat());
            Vec3f v1(shapeSpec["v1"].GetArray()[0].GetFloat(), shapeSpec["v1"].GetArray()[1].GetFloat(), shapeSpec["v1"].GetArray()[2].GetFloat());
            Vec3f v2(shapeSpec["v2"].GetArray()[0].GetFloat(), shapeSpec["v2"].GetArray()[1].GetFloat(), shapeSpec["v2"].GetArray()[2].GetFloat());
            return new Triangle(newid, newMaterial, v0, v1, v2);
        }
        else if (shapetype.compare("trimesh") == 0)
        {
            // Load TriMesh data from .ply file
            if (shapeSpec.HasMember("filename")){
                printf("Loading a trimesh from ply\n");
                std::string filename = shapeSpec["filename"].GetString();
                return new TriMesh(newid, newMaterial, filename);
            }

            // Load TriMesh data from .json file directly
            int vertices = shapeSpec["vertex"].GetInt();
            int faces = shapeSpec["face"].GetInt();
            printf("Loading a trimesh form json, finding %d vertices and %d faces.\n", vertices, faces);
            std::vector<Vec3f> vs;
            std::vector<Vec3i> fs;
            for (int i = 0; i < vertices; i ++){
                const char* id = "v";
                std::string myString = id + std::to_string(i); // concatenate the char and int
                const char* idx = myString.c_str(); //

                Vec3f vertex(shapeSpec[idx].GetArray()[0].GetFloat(), shapeSpec[idx].GetArray()[1].GetFloat(), shapeSpec[idx].GetArray()[2].GetFloat()); 
                vs.push_back(vertex);
            }
            
            for (int i = 0; i < faces; i ++){
                const char* id = "f";
                std::string myString = id + std::to_string(i); // concatenate the char and int
                const char* idx = myString.c_str(); //
                Vec3i face(shapeSpec[idx].GetArray()[0].GetInt(), shapeSpec[idx].GetArray()[1].GetInt(), shapeSpec[idx].GetArray()[2].GetInt()); 
                fs.push_back(face);
            }
            printf("Loading trimesh completed\n");
            return new TriMesh(newid, newMaterial, vs,fs);
        }
        else{
            std::cout<<"Warning: Unknown shape type: " << shapetype <<std::endl;
        }

        return 0;
    }

    boundingBox mergeBox(boundingBox lbox, boundingBox rbox){
		boundingBox box(0,0,0,0,0,0);
		box.xMax = std::max(lbox.xMax,rbox.xMax);
		box.xMin = std::min(lbox.xMin,rbox.xMin);
		box.yMax = std::max(lbox.yMax,rbox.yMax);
		box.yMin = std::min(lbox.yMin,rbox.yMin);
		box.zMax = std::max(lbox.zMax,rbox.zMax);
		box.zMin = std::min(lbox.zMin,rbox.zMin);
		return box;
	}

    bool contains(boundingBox mainbox, boundingBox newbox){
        if (mainbox.xMax >= newbox.xMax && mainbox.xMin <= newbox.xMin 
            && mainbox.yMax >= newbox.yMax && mainbox.yMin <= newbox.yMin
            && mainbox.zMax >= newbox.zMax && mainbox.zMin <= newbox.zMin){
                return true;
            }
        return false;
    }

    bool boxrayintersect(boundingBox box, Ray ray){
        float t,x,y,z;
        t = (box.xMax - ray.origin.x) / ray.direction.x;
        y = ray.origin.y + ray.direction.y * t;
        z = ray.origin.z + ray.direction.z * t;
        if (y>box.yMin && y < box.yMax && z > box.zMin && z < box.zMax){
            return true;
        }

        t = (box.xMin - ray.origin.x) / ray.direction.x;
        y = ray.origin.y + ray.direction.y * t;
        z = ray.origin.z + ray.direction.z * t;
        if (y>box.yMin && y < box.yMax && z > box.zMin && z < box.zMax){
            return true;
        }

        t = (box.yMax - ray.origin.y) / ray.direction.y;
        x = ray.origin.x + ray.direction.x * t;
        z = ray.origin.z + ray.direction.z * t;
        if (x>box.xMin && x < box.xMax && z > box.zMin && z < box.zMax){
            return true;
        }

        t = (box.yMin - ray.origin.y) / ray.direction.y;
        x = ray.origin.x + ray.direction.x * t;
        z = ray.origin.z + ray.direction.z * t;
        if (x>box.xMin && x < box.xMax && z > box.zMin && z < box.zMax){
            return true;
        }

        t = (box.zMax - ray.origin.z) / ray.direction.z;
        x = ray.origin.x + ray.direction.x * t;
        y = ray.origin.y + ray.direction.y * t;
        if (y>box.yMin && y < box.yMax && x > box.xMin && x < box.xMax){
            return true; 
        }

        t = (box.zMin - ray.origin.z) / ray.direction.z;
        x = ray.origin.x + ray.direction.x * t;
        y = ray.origin.y + ray.direction.y * t;
        if (y>box.yMin && y < box.yMax && x > box.xMin && x < box.xMax){
            return true; 
        }

        return false;
    }



} // namespace rt