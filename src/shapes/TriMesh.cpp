/*
 * TriMesh.cpp
 *
 *
 */
#include "TriMesh.h"
#include "shapes/Triangle.h"
#include <math.h>
#include <fstream>
#include <vector> 
#include <sstream> 

namespace rt
{
    using namespace std;

    TriMesh::TriMesh(std::string id, Material *material,std::vector<Vec3f> vs, std::vector<Vec3i> fs)
    {
        this->id = id;
        this->material = material;
        this->vertices = vs;
        this->faces = fs;
    }

    TriMesh::TriMesh(std::string id, Material *material, string filename){
        this->id = id;
        this->material = material;
        
        ifstream plyfile(filename.c_str());
        string line;

        if (plyfile.is_open())
        {
            cout<<"Reading ply file: "<< filename << endl;
            int tri_count, vert_count;
            string s; 
            const char delim = ' ';
            vector<Vec3f> pts;
            while ( getline(plyfile, line) )
            {
                if ( line.compare("end_header") == 0 )
                {
                    break;
                }
                stringstream ss(line); 
                while ( getline(ss, s, delim) )
                {
                    if ( s.compare("vertex") == 0 )
                    {
                        getline(ss, s, delim);
                        vert_count = stoi( s );
                    }

                    if ( s.compare("face") == 0 )
                    {
                        getline(ss, s, delim);
                        tri_count = stoi( s );
                    }
                }
            }

            for ( int i = 0; i < vert_count; i++ )
            {
                getline(plyfile, line);
                stringstream ss(line); 
                getline(ss, s, delim);
                float a = stod( s );
                getline(ss, s, delim);
                float b = stod( s );
                getline(ss, s, delim);
                float c = stod( s );
                this->vertices.push_back(Vec3f(a, b, c));
            }

            for ( int i = 0; i < tri_count; i++ )
            {
                getline(plyfile, line);
                stringstream ss(line); 
                getline(ss, s, delim); 
                getline(ss, s, delim);
                int d = stod( s );
                getline(ss, s, delim);
                int e = stod( s );
                getline(ss, s, delim);
                int f = stod( s );
                this->faces.push_back(Vec3i(d,e,f));
            }

            plyfile.close();
        }
    }

    Hit TriMesh::intersect(Ray ray)
    {

        Hit h = Hit();
        //-----------to be implemented -------------

        Vec3f origin = ray.origin;
        Vec3f dir = ray.direction;

        RayType tarytype = ray.raytype;

        for (int f = 0; f < faces.size(); f++)
        {
            int va = faces[f][0];
            int vb = faces[f][1];
            int vc = faces[f][2];
            Vec3f a = vertices[va];
            Vec3f b = vertices[vb];
            Vec3f c = vertices[vc];

            Hit nextHit = Hit();
            Vec3f origin = ray.origin;
            Vec3f dir = ray.direction;

            RayType raytype = ray.raytype;
            // the while plane of ACB: n * p + d = 0
            // n = AC X AB
            // AC = C - A
            // d = - n * A

            Vec3f AC = c - a;
            Vec3f AB = b - a;
            Vec3f norm = AC.crossProduct(AB);
            float d = -norm.dotProduct(a);

            // ray = origin + t * dir

            // if n * r = 0: parallel, no interact or infinite intersection, ignore
            if (norm.dotProduct(dir) != 0)
            {
                float t = norm.dotProduct(a - origin) / norm.dotProduct(dir);

                Vec3f p = origin + dir * t; // point of intersection
                float r1 = norm.dotProduct((p - a).crossProduct(b - a));
                float r2 = norm.dotProduct((p - b).crossProduct(c - b));
                float r3 = norm.dotProduct((p - c).crossProduct(a - c));

                if ((r1 * r2 > 0) && (r2 * r3 > 0) && (r1 * r3 > 0) && t > 0)
                {
                    float distance = (p - origin).length(); // distance from ray start point to the first interaction point
                    dir = dir.normalize();
                    norm = norm.normalize();
                    // angle = arccos(a * b) / (a.len * b.len)
                    if (dir.dotProduct(norm) > 0){
                        norm = -norm;
                    }
                    float angle = acos((-dir).dotProduct(norm));
                    Vec3f projection = norm * (norm.dotProduct(dir));
                    Vec3f reflectiondir = dir - 2 * projection;
                    nextHit = Hit(p, distance, angle, norm, reflectiondir);
                    //printf("Found a Hit on triangle with angle: %f \n", angle);
                }
            }

            if (nextHit.isHit)
            {
                if (!h.isHit)
                {
                    h = nextHit;
                }
                else if (nextHit.distance < h.distance)
                {
                    h = nextHit;
                }
            }
        }
        return h;
    }

    Vec3f TriMesh::getShadowColor(Ray incomingray, Vec3f lightpos, Vec3f lightid, Vec3f lightis,  Hit hitPoint, JPGLoader *loader)
    {
        // first found the triangle hit
        Hit h = Hit();
        Vec3f A;
        Vec3f B;
        Vec3f C;
        for (int f = 0; f < faces.size(); f++)
        {
            int va = faces[f][0];
            int vb = faces[f][1];
            int vc = faces[f][2];
            Vec3f a = vertices[va];
            Vec3f b = vertices[vb];
            Vec3f c = vertices[vc];

            Hit nextHit = Hit();

            Vec3f origin = incomingray.origin;
            Vec3f dir = incomingray.direction;

            RayType raytype = incomingray.raytype;
            // the while plane of ACB: n * p + d = 0
            // n = AC X AB
            // AC = C - A
            // d = - n * A

            Vec3f AC = c - a;
            Vec3f AB = b - a;
            Vec3f norm = AC.crossProduct(AB);
            float d = -norm.dotProduct(a);
            // ray = origin + t * dir
            // if n * r = 0: parallel, no interact or infinite intersection, ignore
            if (norm.dotProduct(dir) == 0)continue;
            // t = n * (A-O) / (n * r)
            float t = norm.dotProduct(a - origin) / norm.dotProduct(dir);
            Vec3f p = origin + dir * t; // point of intersection
            // check if inside Plane ABC :
            // r1 = n * (AP X AB)
            float r1 = norm.dotProduct((p - a).crossProduct(b - a));
            float r2 = norm.dotProduct((p - b).crossProduct(c - b));
            float r3 = norm.dotProduct((p - c).crossProduct(a - c));

            if ((r1 * r2 > 0) && (r2 * r3 > 0) && t > 0)
            {
                // intersection with plane at p confirmed

                float distance = (p - origin).length(); // distance from ray start point to the first interaction point
                dir = dir.normalize();
                norm = norm.normalize();
                // angle = arccos(a * b) / (a.len * b.len)
                if (dir.dotProduct(norm) > 0)
                {
                    norm = -norm;
                }
                float angle = acos((-dir).dotProduct(norm));
                Vec3f projection = norm * (norm.dotProduct(dir));
                Vec3f reflectiondir = dir - 2 * projection;
                nextHit = Hit(p, distance, angle, norm, reflectiondir);
                // printf("Found a Hit on triangle with angle: %f \n", angle);
            }

            if (nextHit.isHit)
            {
                if (!h.isHit)
                {
                    h = nextHit;
                    A = a;
                    B = b;
                    C = c;
                }
                else if (nextHit.distance < h.distance)
                {
                    h = nextHit;
                    A = a;
                    B = b;
                    C = c;
                }
            }
        }

        // get the color from the triangle
        Vec3f diff = material->getDiffuseColor(incomingray, lightpos,lightid, hitPoint);
        if (!material->tPath._Equal(""))
        {
            int w = material->tWidth;
            int h = material->tHeight;

            Vec3f P = hitPoint.point;
            Vec3f AB = B - A;				//AB as u axis
			Vec3f AC = C - A;
            Vec3f AP = P - A;
            float ac_len = AC.length();
			Vec3f n = AB.crossProduct(AC);
			AC = n.crossProduct(AB).normalize() * ac_len;		//AC as v axis

            float u = AP.dotProduct(AB) / AB.length();
			float v = AP.dotProduct(AC) / AC.length();
            u = u * w;
            int u_valid = u;
			if (u_valid > w) u_valid = u_valid % w;
			if (u_valid < 0) u_valid = u_valid % w + w;
			v = v * h;
			int v_valid = v;
			if (v_valid > h) v_valid = v_valid % h;
			if (v_valid < 0) v_valid = v_valid % h + h;
            Vec3f pixelcolor = loader->getValue(material->tPath, w, h, u_valid, v_valid);
            diff = pixelcolor * (diff.length() / material->diffusecolor.length());
        }
        Vec3f spec = material->getSpecularColor(incomingray, lightpos,lightis, hitPoint);
        Vec3f res = diff + spec;
		res.x = std::max(res.x, 0.0f);
		res.y = std::max(res.y, 0.0f);
		res.z = std::max(res.z, 0.0f);
		return res;
    }

    Vec3f TriMesh::getDistributedDiffColor(Ray incomingray, Vec3f raydir,  Vec3f lightcolor, Hit hitPoint, JPGLoader *loader)
    {
        // first found the triangle hit
        Hit h = Hit();
        Vec3f A;
        Vec3f B;
        Vec3f C;
        for (int f = 0; f < faces.size(); f++)
        {
            int va = faces[f][0];
            int vb = faces[f][1];
            int vc = faces[f][2];
            Vec3f a = vertices[va];
            Vec3f b = vertices[vb];
            Vec3f c = vertices[vc];

            Hit nextHit = Hit();

            Vec3f origin = incomingray.origin;
            Vec3f dir = incomingray.direction;

            RayType raytype = incomingray.raytype;
            // the while plane of ACB: n * p + d = 0
            // n = AC X AB
            // AC = C - A
            // d = - n * A

            Vec3f AC = c - a;
            Vec3f AB = b - a;
            Vec3f norm = AC.crossProduct(AB);
            float d = -norm.dotProduct(a);
            // ray = origin + t * dir
            // if n * r = 0: parallel, no interact or infinite intersection, ignore
            if (norm.dotProduct(dir) == 0)continue;
            // t = n * (A-O) / (n * r)
            float t = norm.dotProduct(a - origin) / norm.dotProduct(dir);
            Vec3f p = origin + dir * t; // point of intersection
            // check if inside Plane ABC :
            // r1 = n * (AP X AB)
            float r1 = norm.dotProduct((p - a).crossProduct(b - a));
            float r2 = norm.dotProduct((p - b).crossProduct(c - b));
            float r3 = norm.dotProduct((p - c).crossProduct(a - c));

            if ((r1 * r2 > 0) && (r2 * r3 > 0) && t > 0)
            {
                // intersection with plane at p confirmed

                float distance = (p - origin).length(); // distance from ray start point to the first interaction point
                dir = dir.normalize();
                norm = norm.normalize();
                // angle = arccos(a * b) / (a.len * b.len)
                if (dir.dotProduct(norm) > 0)
                {
                    norm = -norm;
                }
                float angle = acos((-dir).dotProduct(norm));
                Vec3f projection = norm * (norm.dotProduct(dir));
                Vec3f reflectiondir = dir - 2 * projection;
                nextHit = Hit(p, distance, angle, norm, reflectiondir);
                // printf("Found a Hit on triangle with angle: %f \n", angle);
            }

            if (nextHit.isHit)
            {
                if (!h.isHit)
                {
                    h = nextHit;
                    A = a;
                    B = b;
                    C = c;
                }
                else if (nextHit.distance < h.distance)
                {
                    h = nextHit;
                    A = a;
                    B = b;
                    C = c;
                }
            }
        }

        // get the color from the triangle
        Vec3f diff = material->getDistributedDiff(incomingray, raydir, lightcolor, hitPoint);
        if (!material->tPath._Equal(""))
        {
            int w = material->tWidth;
            int h = material->tHeight;

            Vec3f P = hitPoint.point;
            Vec3f AB = B - A;				//AB as u axis
			Vec3f AC = C - A;
            Vec3f AP = P - A;
			Vec3f n = AB.crossProduct(AC);
			AC = n.crossProduct(AB);		//AC as v axis
            // since p is on plane ABC, P = A + u * AB + V * AD
            float u = AP.dotProduct(AB) / AB.length();
			float v = AP.dotProduct(AC) / AC.length();
            u = u * w;
            int u_valid = u;
			if (u_valid > w) u_valid = u_valid % w;
			if (u_valid < 0) u_valid = u_valid % w + w;
			v = v * h;
			int v_valid = v;
			if (v_valid > h) v_valid = v_valid % h;
			if (v_valid < 0) v_valid = v_valid % h + h;
            Vec3f pixelcolor = loader->getValue(material->tPath, w, h, u_valid, v_valid);
            diff = pixelcolor * (diff.length() / material->diffusecolor.length());
        }
		diff.x = std::max(diff.x, 0.0f);
		diff.y = std::max(diff.y, 0.0f);
		diff.z = std::max(diff.z, 0.0f);
		return diff;
    }


    Vec3f TriMesh::getDistributedSpecColor(Ray incomingray, Vec3f raydir,  Vec3f lightcolor, Hit hitPoint, JPGLoader *loader)
    {
        Vec3f res = material->getDistributedSpec(incomingray, raydir, lightcolor, hitPoint);
		res.x = std::max(res.x, 0.0f);
		res.y = std::max(res.y, 0.0f);
		res.z = std::max(res.z, 0.0f);
		return res;
    }

    boundingBox TriMesh::getBound(){
		float xMax, xMin, yMax, yMin, zMax, zMin;
        xMax = vertices[0].x;
        xMin = vertices[0].x;
        yMax = vertices[0].y;
        yMin = vertices[0].y;
        zMax = vertices[0].z;
        zMin = vertices[0].z;

        for (int i = 0; i < vertices.size(); i++){
            xMax = std::max(xMax, vertices[i].x);
            xMin = std::min(xMin, vertices[i].x);
            yMax = std::max(yMax, vertices[i].y);
            yMin = std::min(yMin, vertices[i].y);
            zMax = std::max(zMax, vertices[i].z);
            zMin = std::min(zMin, vertices[i].z);
        }
		return boundingBox(xMax, xMin, yMax, yMin, zMax, zMin);
	}

    Vec3f TriMesh::getRandomPoint(){
        int f = std::rand() / ((RAND_MAX + 1) / faces.size());
        f = std::min(f, (int)faces.size() - 1);
        Vec3i face = faces[f];
        Vec3f v0 = vertices[face[0]];
        Vec3f v1 = vertices[face[1]];
        Vec3f v2 = vertices[face[2]];

		Vec3f AB = v1 - v0;
		Vec3f AC = v2 - v0;
		Vec3f norm = AC.crossProduct(AB);
		Vec3f p;
        float x = std::rand() * 1.0f / RAND_MAX;
        float y = std::rand() * 1.0f / RAND_MAX;
		p = v0 + AB * x + AC * y;
		float r1 = norm.dotProduct((p - v0).crossProduct(v1 - v0));
		float r2 = norm.dotProduct((p - v1).crossProduct(v2 - v1));
		float r3 = norm.dotProduct((p - v2).crossProduct(v0 - v2));
		if (!(r1 * r2 > 0) || !(r2 * r3 > 0) || !(r1 * r3 > 0)) p = v0 + AB * (1-x) + AC * (1-y);
		return p;
    }

    TriMesh::~TriMesh()
    {
        delete this->material;
    }

} // namespace rt
