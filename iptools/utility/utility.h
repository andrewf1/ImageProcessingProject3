#ifndef UTILITY_H
#define UTILITY_H

#include "../image/image.h"
#include <sstream>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <utility> 
#include <vector>
#include "../../project/roi.hpp"
#include "../../project/color.hpp"
using namespace std;

struct gradient_amplitude {
	int gx, gy;
};

class utility
{
	public:
		utility();
		virtual ~utility();
		static std::string intToString(int number);
		static int checkValue(int value);
		static void addGrey(image &src, image &tgt, int value);
		static void binarize(image &src, image &tgt, int threshold);
		static void scale(image &src, image &tgt, float ratio);
		static void increaseBrightness(image& src, image& tgt, const int& threshold, const int& intensity);
		static void grayEdgeDetection(image& src, image& tgt, const vector<roi>& regions, char* outfile);
		static void colorEdgeDetection(image& src, image& tgt, const vector<roi>& regions);
};

#endif

