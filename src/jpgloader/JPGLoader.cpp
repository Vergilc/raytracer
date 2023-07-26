/*
 * JPGLoader.cpp
 *
 *
 */
#include "JPGLoader.h"


#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace rt
{


    JPGLoader::JPGLoader(std::vector<std::string> filenames, std::vector<unsigned char *> datavec, int channels):filenames(filenames),datavec(datavec), channels(channels){};

    JPGLoader* JPGLoader::createJPGLoader(){
        std::vector<std::string> files = {};
        std::vector<unsigned char *> data = {};
        return new JPGLoader(files,data,3);
    }

    void JPGLoader::loadJPG(std::string filename){
        std::cout <<"Loading new JPG file: "<< filename <<"\n";
        filenames.push_back(filename);
        const char * file = filename.c_str();
        Vec3f color = Vec3f(0,0,0);
        // load an image with 3 channels (RGB)
        int width = 0;
        int height = 0;
        unsigned char * data = stbi_load(file, &width, &height, &channels, 3);
        
        if (data == NULL) {
            std::cerr << "Error loading image\n";
        }
        datavec.push_back(data);
        std::cout << "Image width = " << width << "\n";
        std::cout << "Image height = " << height << "\n";
    }

    Vec3f JPGLoader::getValue(std::string filename, int w, int h, int u, int v){
        int idx = 0;
        bool found = false;
        for (int i = 0; i < filenames.size(); i++){
            if (filenames[i]._Equal(filename)){
                idx = i;
                found = true;
                break;
            }
        }
        if (!found){
            loadJPG(filename);
            idx = filenames.size() - 1;
        }

        unsigned char * data = datavec[idx];

        //std::cout << "Getting pixel at u = " << u << "\n";
        while (u > w) u -= w;
        while (u < 0) u += w;
        while (v > h) v -= h;
        while (v < 0) v += h;

        // calculate the index of the pixel in data array
        int index = (u + v * w) * channels;
        
        // get RGB values from data array
        unsigned char r = data[index];
        unsigned char g = data[index + 1];
        unsigned char b = data[index + 2];
        
        int r_value = r;
        int g_value = g;
        int b_value = b;

        float rf = r_value / 255.f;
        float gf = g_value / 255.f;
        float bf = b_value / 255.f;

        Vec3f color = Vec3f(rf,gf,bf);
        return color;
    }

    JPGLoader::~JPGLoader(){
        for (int i = 0; i < datavec.size(); i++){
            stbi_image_free(datavec[i]);
        }
    }

}

#endif
