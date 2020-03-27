#ifndef UTILITY_H
#define UTILITY_H

#include "../image/image.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <utility> 
#include <vector>
#include "../../project/roi.hpp"
#include "../../project/color.hpp"
using namespace std;

const vector<vector<int>> gradient_x_kernal{
	{-1, 0, 1},
	{-2, 0, 2},
	{1, 0, 1}
};

const vector<vector<int>> gradient_y_kernal{
	{-1, -2, -1},
	{0, 0, 0},
	{1, 2, 1}
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
																							// dont think i need these two...
		static void grayEdgeDetection(image& src, image& tgt, const vector<roi>& regions);
		static void colorEdgeDetection(image& src, image& tgt, const vector<roi>& regions);
};

#endif

