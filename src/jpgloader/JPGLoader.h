/*
 * JPGLoader.h
 *
 */

#ifndef JPGLOADER_H_
#define JPGLOADER_H_

#include <vector>

#include "math/geometry.h"
#include "stb_image.h"

namespace rt{

class JPGLoader {
public:
    JPGLoader(){};
    JPGLoader(std::vector<std::string> filenames, std::vector<unsigned char *> datavec, int channels);
    
    ~JPGLoader();

    static JPGLoader* createJPGLoader();

    void loadJPG(std::string filename);

    Vec3f getValue(std::string filename,int w, int h, int u, int v );

private:
    std::vector<std::string> filenames;
    std::vector<unsigned char *> datavec;
    int channels = 3;
};

} //namespace rt

#endif /* JPGLOADER_H_ */
