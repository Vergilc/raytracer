/*
 * main.cpp
 *
 */

#include <iostream>
#include <fstream>
#include <ctime>

#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"

#include "math/geometry.h"
#include "parsers/PPMWriter.h"

#include "core/RayTracer.h"
#include "core/Camera.h"
#include "core/Scene.h"


using namespace rt;
using namespace rapidjson;

int main(int argc, char* argv[]){

	//parse commandline arguments
	char* inputFile=argv[1];    //first command line argument holds the path to the json input file
	char* outputFile=argv[2];   //second command line argument holds the path to the output image file

	if (inputFile == NULL){
		printf("Error: No input file specified!\n");
		exit(1);
	}
	
	if (outputFile == NULL){
		printf("Error: No output file specified!\n");
		exit(1);
	}

	std::printf("Input file: %s\n",inputFile);

	//parse json input file using rapidjson
	std::ifstream ifs(inputFile);
	IStreamWrapper is(ifs);
	Document d;     // d holds the complete json object
	d.ParseStream(is);

	//generate a camera according to the input file
	Camera* camera=Camera::createCamera(d["camera"]);

	//print camera data (based on the input file provided)
	camera->printCamera();

	printf("Creating scene\n");
	//generate the scene according to the input file
	Scene* scene=new Scene();
	scene->createScene(d["scene"]);

	std::time_t result = std::time(nullptr);
	std::asctime(std::localtime(&result));

	Vec3f* pixelbuffer=RayTracer::render(camera, scene, d["nbounces"].GetInt());

	printf("Start tonemapping.\n");
	RayTracer::tonemap(pixelbuffer, camera);

	std::printf("Output file: %s\n",outputFile);
	
	PPMWriter::PPMWriter(camera->getWidth(), camera->getHeight(), pixelbuffer, outputFile);

	printf("PPM generated, closing.\n");

	std::time_t result_ano = std::time(nullptr);
	std::asctime(std::localtime(&result_ano));
	std::cout << result_ano - result << " seconds taken for rendering: "<< outputFile <<" \n";
	printf("<--------------------------------------------------->\n");
	
	delete pixelbuffer;
	
	delete camera;
	delete scene;
}


