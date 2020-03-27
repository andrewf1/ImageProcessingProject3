#include "utility.h"

#define MAXRGB 255
#define MINRGB 0
#define THRESH_LIMIT 5

double colorEuclideanDistance(color c1, color c2) {
	int red_part = c2.R - c1.R;
	red_part *= red_part;
	int green_part = c2.G - c1.G;
	green_part *= green_part;
	int blue_part = c2.B - c1.B;
	blue_part *= blue_part;
	return sqrt(red_part + green_part + blue_part);
}

std::string utility::intToString(int number)
{
   std::stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

int utility::checkValue(int value)
{
	if (value > MAXRGB)
		return MAXRGB;
	if (value < MINRGB)
		return MINRGB;
	return value;
}

/*-----------------------------------------------------------------------**/
void utility::addGrey(image &src, image &tgt, int value)
{
	tgt.resize(src.getNumberOfRows(), src.getNumberOfColumns());
	for (int i=0; i<src.getNumberOfRows(); i++)
		for (int j=0; j<src.getNumberOfColumns(); j++)
		{
			tgt.setPixel(i,j,checkValue(src.getPixel(i,j)+value)); 
		}
}

/*-----------------------------------------------------------------------**/
void utility::binarize(image &src, image &tgt, int threshold)
{
	tgt.resize(src.getNumberOfRows(), src.getNumberOfColumns());
	for (int i=0; i<src.getNumberOfRows(); i++)
	{
		for (int j=0; j<src.getNumberOfColumns(); j++)
		{
			if (src.getPixel(i,j) < threshold)
				tgt.setPixel(i,j,MINRGB);
			else
				tgt.setPixel(i,j,MAXRGB);
		}
	}
}

/*-----------------------------------------------------------------------**/
void utility::scale(image &src, image &tgt, float ratio)
{
	int rows = (int)((float)src.getNumberOfRows() * ratio);
	int cols  = (int)((float)src.getNumberOfColumns() * ratio);
	tgt.resize(rows, cols);
	for (int i=0; i<rows; i++)
	{
		for (int j=0; j<cols; j++)
		{	
			/* Map the pixel of new image back to original image */
			int i2 = (int)floor((float)i/ratio);
			int j2 = (int)floor((float)j/ratio);
			if (ratio == 2) {
				/* Directly copy the value */
				tgt.setPixel(i,j,checkValue(src.getPixel(i2,j2)));
			}

			if (ratio == 0.5) {
				/* Average the values of four pixels */
				int value = src.getPixel(i2,j2) + src.getPixel(i2,j2+1) + src.getPixel(i2+1,j2) + src.getPixel(i2+1,j2+1);
				tgt.setPixel(i,j,checkValue(value/4));
			}
		}
	}
}

/*-----------------------------------------------------------------------**/
void utility::increaseBrightness(image& src, image& tgt, const int& threshold, const int& intensity)  {
	tgt.resize(src.getNumberOfRows(), src.getNumberOfColumns());

	for (int i = 0; i < src.getNumberOfRows(); i++) {
		for (int j = 0; j < src.getNumberOfColumns(); j++){
			if (src.getPixel(i, j) > threshold) { // if the pixel's intensity is greater than the treshold
				// increase intensity with value
				tgt.setPixel(i, j, checkValue(src.getPixel(i, j) + intensity));
			}
			else {
				// copies the pixel from src to tgt
				tgt.setPixel(i, j, checkValue(src.getPixel(i, j)));
			}
		}
	}
}

/*-----------------------------------------------------------------------**/
bool isIndexInRegion(const roi& region, const int& i, const int& j) {
	return i >= region.y && 
		i < (region.y + region.sy) &&
		j >= region.x &&
		j < (region.x + region.sx);
}

// returns a pair, first is the X gradient, second is the Y gradient
pair<int, int> getGradientXY(const vector<vector<int>>& pixelVals) {
	int x_grad = (
		(pixelVals[0][0] * gradient_x_kernal[0][0]) +
		(pixelVals[0][1] * gradient_x_kernal[0][1]) +
		(pixelVals[0][2] * gradient_x_kernal[0][2]) +
		(pixelVals[1][0] * gradient_x_kernal[1][0]) +
		(pixelVals[1][1] * gradient_x_kernal[1][1]) +
		(pixelVals[1][2] * gradient_x_kernal[1][2]) +
		(pixelVals[2][0] * gradient_x_kernal[2][0]) +
		(pixelVals[2][1] * gradient_x_kernal[2][1]) +
		(pixelVals[2][2] * gradient_x_kernal[2][2])
	);

	int y_grad = (
		(pixelVals[0][0] * gradient_y_kernal[0][0]) +
		(pixelVals[0][1] * gradient_y_kernal[0][1]) +
		(pixelVals[0][2] * gradient_y_kernal[0][2]) +
		(pixelVals[1][0] * gradient_y_kernal[1][0]) +
		(pixelVals[1][1] * gradient_y_kernal[1][1]) +
		(pixelVals[1][2] * gradient_y_kernal[1][2]) +
		(pixelVals[2][0] * gradient_y_kernal[2][0]) +
		(pixelVals[2][1] * gradient_y_kernal[2][1]) +
		(pixelVals[2][2] * gradient_y_kernal[2][2])
	);

	return make_pair(x_grad, y_grad);
}

void utility::grayEdgeDetection(image& src, image& tgt, const vector<roi>& regions, const int& threshold, const int& direction) {
	tgt.resize(src.getNumberOfRows(), src.getNumberOfColumns());
	image temp_img;
	temp_img.copyImage(src);
	for (int r = 0; r < regions.size(); r++) {
		int x = regions.at(r).x;
		int y = regions.at(r).y;
		int sx = regions.at(r).sx;
		int sy = regions.at(r).sy;
		// int thresh = regions.at(r).gray_threshold;
		// int dir = regions.at(r).gray_direction;
		for (int i = 0; i < temp_img.getNumberOfRows(); i++) {
			for (int j = 0; j < temp_img.getNumberOfColumns(); j++) {
				if (
					i >= y && 
					i < (y + sy) &&
					j >= x &&
					j < (x + sx)
				) { // inside the region
					int curr_pixel = temp_img.getPixel(i, j); // idx [1][1]
					vector<vector<int>> pixelHoodVals;
					for (int row = 0; row < 3; row++) {
						for (int col = 0; col < pixelHoodVals.at(row).size(); col++) {
							
							pixelHoodVals.at(row).at(col) = 0;
						}
					}
					
					for (int k = i - 1; k < i + 1; k++) {
						for (int l = j - 1; l < j + 1; j++) {
							if (isIndexInRegion(regions.at(r), k, j)) {
								if (k == i - 1 && l == j - 1) {
									pixelHoodVals.at(0).at(0) = temp_img.getPixel(i - 1, j -1);
								}
								else if (k == i && l == j - 1) {
									pixelHoodVals.at(0).at(1) = temp_img.getPixel(i, j - 1);
								}
								else if (k == i + 1 && l == j - 1) {
									pixelHoodVals.at(0).at(2) = temp_img.getPixel(i + 1, j - 1);
								}
								else if (k == i - 1 && l == j) {
									pixelHoodVals.at(1).at(0) = temp_img.getPixel(i - 1, j);
								}
								else if (k == i && l == j) {
									pixelHoodVals.at(1).at(1) = temp_img.getPixel(i, j);
								}
								else if (k == i + 1 && l == j) {
									pixelHoodVals.at(1).at(2) = temp_img.getPixel(i + 1, j);
								}
								else if (k == i - 1 && l == j + 1) {
									pixelHoodVals.at(2).at(0) = temp_img.getPixel(i - 1, j + 1);
								}
								else if (k == i && l == j + 1) {
									pixelHoodVals.at(2).at(1) = temp_img.getPixel(i, j + 1);
								}
								else if (k == i + 1 && l == j + 1) {
									pixelHoodVals.at(2).at(2) = temp_img.getPixel(i + 1, j + 1);
								}
							}
						}
					}

					int x_gradient = getGradientXY(pixelHoodVals).first;
					int y_gradient = getGradientXY(pixelHoodVals).second;
					// HOW TO NORMALIZE THIS
					double gradient_amplitude = sqrt(pow(x_gradient, 2) + pow(y_gradient, 2));
					double dir = atan(y_gradient/x_gradient);

					temp_img.setPixel(i, j, gradient_amplitude);
				}
			}
		}
	}
}

/*-----------------------------------------------------------------------**/
void::utility::colorEdgeDetection(image& src, image& tgt, const vector<roi>& regions, const int& threshold, const int& direction) {

}