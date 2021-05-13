#ifndef IMAGE_H
#define IMAGE_H
#include <vector>
#include "RGB.h"
using namespace std;
class ImageType {
	int N, M, Q; //Rows, Columns, Levels
	RGB **pixelValue;
public:
	ImageType();
	~ImageType() { };
	void getImageInfo(int&, int&, int&);
	void setImageInfo(int, int, int);
	void setPixelVal(int, int, RGB);
	void getPixelVal(int, int, RGB&);
};

#include "image.cpp"
#endif
