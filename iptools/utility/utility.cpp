#include "utility.h"

#define MAXRGB 255
#define MINRGB 0
#define PI 3.14159265358979323846
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
int getPixelIfInROI(image& src, int i, int j, roi& reg) {
	if (
		i >= reg.y && 
		i < (reg.y + reg.sy) &&
		j >= reg.x &&
		j < (reg.x + reg.sx)
	) {	// In the ROI
		return src.getPixel(i, j);
	}
	else {
		return 0;
	}
}

gradient_amplitude getGradientXY(image& src, int i, int j, roi reg) {
	int x_grad = 0, y_grad = 0;
	gradient_amplitude ga;

	x_grad = (
		getPixelIfInROI(src, i + 1, j - 1, reg) +
		getPixelIfInROI(src, i + 1, j, reg) * 2 +
		getPixelIfInROI(src, i + 1, j + 1, reg) -
		getPixelIfInROI(src, i - 1, j - 1, reg) -
		getPixelIfInROI(src, i - 1, j, reg) * 2 -
		getPixelIfInROI(src, i - 1, j + 1, reg)
	);

	y_grad = (
		getPixelIfInROI(src, i + 1, j + 1, reg) +
		getPixelIfInROI(src, i, j + 1, reg) * 2 +
		getPixelIfInROI(src, i - 1, j + 1, reg) -
		getPixelIfInROI(src, i - 1, j - 1, reg) -
		getPixelIfInROI(src, i, j - 1, reg) * 2 -
		getPixelIfInROI(src, i + 1, j - 1, reg) 
	);

	x_grad /= 8;
	ga.gx = x_grad;

	y_grad /= 8;
	ga.gy = y_grad;

	return ga;
}

void utility::grayEdgeDetection(image& src, image& tgt, const vector<roi>& regions, char* outfile) {
	tgt.resize(src.getNumberOfRows(), src.getNumberOfColumns());
	
	image amplitude_threshold, direction_threshold;
	amplitude_threshold.resize(src.getNumberOfRows(), src.getNumberOfColumns());
	direction_threshold.resize(src.getNumberOfRows(), src.getNumberOfColumns());

	image temp_img, temp_img_gat, temp_img_dir;
	double gradient_amplitude;
	double pixel_angle;

	//  sobel operator
	temp_img.copyImage(src);
	temp_img_gat.copyImage(src);
	temp_img_dir.copyImage(src);
	for (int r = 0; r < regions.size(); r++) {
		int x = regions.at(r).x;
		int y = regions.at(r).y;
		int sx = regions.at(r).sx;
		int sy = regions.at(r).sy;
		int T = regions.at(r).gray_threshold;
		int angle = regions.at(r).gray_direction;

		for (int i = 0; i < temp_img.getNumberOfRows(); i++) {
			for (int j = 0; j < temp_img.getNumberOfColumns(); j++) {
				if (
					i >= y && 
					i < (y + sy) &&
					j >= x &&
					j < (x + sx)
				) { // inside the region
					int x_gradient = getGradientXY(src, i, j, regions.at(r)).gx;
					int y_gradient = getGradientXY(src, i, j, regions.at(r)).gy;

					gradient_amplitude = sqrt(pow(x_gradient, 2) + pow(y_gradient, 2));
					pixel_angle = atan((double)y_gradient/(double)x_gradient) * (180/PI);

					// setting intensity image
					tgt.setPixel(i, j, checkValue(gradient_amplitude));

					// setting amplitude thresholed img
					if (gradient_amplitude < T) {
						amplitude_threshold.setPixel(i, j, MINRGB);
					}
					else {
						amplitude_threshold.setPixel(i, j, MAXRGB);
					}

					// setting direction threshold image
					if (gradient_amplitude > T) {
						if (
							pixel_angle >= (angle - 10) &&
							pixel_angle <= (angle + 10)
						) {
							direction_threshold.setPixel(i, j, MAXRGB);
						}
						else {
							direction_threshold.setPixel(i, j, MINRGB);
						}
					}
				}
				else {
					tgt.setPixel(i, j, checkValue(temp_img.getPixel(i, j)));
					amplitude_threshold.setPixel(i, j, checkValue(temp_img_gat.getPixel(i, j)));
					direction_threshold.setPixel(i, j, checkValue(temp_img_dir.getPixel(i, j)));
				}
			}
		}
		temp_img.copyImage(tgt);
		temp_img_gat.copyImage(amplitude_threshold);
		temp_img_dir.copyImage(direction_threshold);
	}
 
	char gat_img_name[100] = "grad_amplitude_thresh_";
	amplitude_threshold.save(strcat(gat_img_name, outfile));

	char dir_img_name[100] = "direction_thresh_";
	direction_threshold.save(strcat(dir_img_name, outfile));
}

/*-----------------------------------------------------------------------**/
void::utility::colorEdgeDetection(image& src, image& tgt, const vector<roi>& regions) {

}