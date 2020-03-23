#ifndef UTILITY_H
#define UTILITY_H

#include "../image/image.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <vector>
#include "../../project/roi.hpp"
#include "../../project/color.hpp"
using namespace std;

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
		static void histStretch(image& src, image& tgt, const vector<roi>& regions, string file_name);
		static void optimalThresholding(image& src, image& tgt, const vector<roi>& regions);
		static void recursiveOptThresh(image& src, image& tgt, const roi& reg, const vector<int>& pixelVals, const double& thresh);
		static void combHistOptThresh(image& src, image& tgt, const vector<roi>& regions, string file_name);
		static void histRed(image& src, image& tgt, const vector<roi>& regions, string file_name);
		static void histGreen(image& src, image& tgt, const vector<roi>& regions, string file_name);
		static void histBlue(image& src, image& tgt, const vector<roi>& regions, string file_name);
		static void histRGB(image& src, image& tgt, const vector<roi>& regions, string file_name);
};

#endif

