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
					// calculating gx, gy, gradient amplitude, and edge direction using Sobel Operator 3x3
					int x_gradient = getGradientXY(temp_img, i, j, regions.at(r)).gx;
					int y_gradient = getGradientXY(temp_img, i, j, regions.at(r)).gy;

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
// overloaded function to get the pixel if in ROI when also given a particular color
int getPixelIfInROI(image& src, int i, int j, roi& reg, channel color) {
	if (
		i >= reg.y && 
		i < (reg.y + reg.sy) &&
		j >= reg.x &&
		j < (reg.x + reg.sx)
	) {	// In the ROI
		return src.getPixel(i, j, color);
	}
	else {
		return 0;
	}
}

// overloaded function to get the gradient amplitude
gradient_amplitude getGradientXY(image& src, int i, int j, roi reg, channel color) {
	int x_grad = 0, y_grad = 0;
	gradient_amplitude ga;

	x_grad = (
		getPixelIfInROI(src, i + 1, j - 1, reg, color) +
		getPixelIfInROI(src, i + 1, j, reg, color) * 2 +
		getPixelIfInROI(src, i + 1, j + 1, reg, color) -
		getPixelIfInROI(src, i - 1, j - 1, reg, color) -
		getPixelIfInROI(src, i - 1, j, reg, color) * 2 -
		getPixelIfInROI(src, i - 1, j + 1, reg, color)
	);

	y_grad = (
		getPixelIfInROI(src, i + 1, j + 1, reg, color) +
		getPixelIfInROI(src, i, j + 1, reg, color) * 2 +
		getPixelIfInROI(src, i - 1, j + 1, reg, color) -
		getPixelIfInROI(src, i - 1, j - 1, reg, color) -
		getPixelIfInROI(src, i, j - 1, reg, color) * 2 -
		getPixelIfInROI(src, i + 1, j - 1, reg, color) 
	);

	x_grad /= 8;
	ga.gx = x_grad;

	y_grad /= 8;
	ga.gy = y_grad;

	return ga;
}

void::utility::RGBEdgeDetection(image& src, image& tgt, const vector<roi>& regions, char* outfile) {
	tgt.resize(src.getNumberOfRows(), src.getNumberOfColumns());
	image red_edge_detection, green_edge_detection, blue_edge_detection;
	red_edge_detection.resize(src.getNumberOfRows(), src.getNumberOfColumns());
	green_edge_detection.resize(src.getNumberOfRows(), src.getNumberOfColumns());
	blue_edge_detection.resize(src.getNumberOfRows(), src.getNumberOfColumns());

	image temp_img, temp_img_red, temp_img_green, temp_img_blue;
	temp_img.copyImage(src);
	temp_img_red.copyImage(src);
	temp_img_green.copyImage(src);
	temp_img_blue.copyImage(src);

	for (int r = 0; r < regions.size(); r++) {
		int x = regions.at(r).x;
		int y = regions.at(r).y;
		int sx = regions.at(r).sx;
		int sy = regions.at(r).sy;
		int T = regions.at(r).color_threshold;
		int angle = regions.at(r).color_direction;

		double red_gradient_amplitude, green_gradient_amplitude, blue_gradient_amplitude;
		double red_pixel_angle, green_pixel_angle, blue_pixel_angle;

		for (int i = 0; i < temp_img.getNumberOfRows(); i++) {
			for (int j = 0; j < temp_img.getNumberOfColumns(); j++) {
				if (
					i >= y && 
					i < (y + sy) &&
					j >= x &&
					j < (x + sx)
				) { // inside the region
					int red_gx = getGradientXY(temp_img, i, j, regions.at(r), RED).gx;
					int red_gy = getGradientXY(temp_img, i, j, regions.at(r), RED).gy;
					int green_gx = getGradientXY(temp_img, i, j, regions.at(r), GREEN).gx;
					int green_gy = getGradientXY(temp_img, i, j, regions.at(r), GREEN).gy;
					int blue_gx = getGradientXY(temp_img, i, j, regions.at(r), BLUE).gx;
					int blue_gy = getGradientXY(temp_img, i, j, regions.at(r), BLUE).gy;

					red_gradient_amplitude = sqrt(pow(red_gx, 2) + pow(red_gy, 2));
					green_gradient_amplitude = sqrt(pow(green_gx, 2) + pow(green_gy, 2));
					blue_gradient_amplitude = sqrt(pow(blue_gx, 2) + pow(blue_gy, 2));

					red_pixel_angle = atan((double)red_gy/(double)red_gx) * (180/PI);
					green_pixel_angle = atan((double)green_gy/(double)green_gx) * (180/PI);
					blue_pixel_angle = atan((double)blue_gy/(double)blue_gx) * (180/PI);

					// thresholding gradient amplitude for three channels
					if (red_gradient_amplitude < T) {
						red_edge_detection.setPixel(i, j, RED, MINRGB);
						red_edge_detection.setPixel(i, j, GREEN, MINRGB);
						red_edge_detection.setPixel(i, j, BLUE, MINRGB);
					}
					else {
						red_edge_detection.setPixel(i, j, RED, MAXRGB);
						red_edge_detection.setPixel(i, j, GREEN, MAXRGB);
						red_edge_detection.setPixel(i, j, BLUE, MAXRGB);
					}

					if (green_gradient_amplitude < T) {
						green_edge_detection.setPixel(i, j, RED, MINRGB);
						green_edge_detection.setPixel(i, j, GREEN, MINRGB);
						green_edge_detection.setPixel(i, j, BLUE, MINRGB);
					}
					else {
						green_edge_detection.setPixel(i, j, RED, MAXRGB);
						green_edge_detection.setPixel(i, j, GREEN, MAXRGB);
						green_edge_detection.setPixel(i, j, BLUE, MAXRGB);
					}

					if (blue_gradient_amplitude < T) {
						blue_edge_detection.setPixel(i, j, RED, MINRGB);
						blue_edge_detection.setPixel(i, j, GREEN, MINRGB);
						blue_edge_detection.setPixel(i, j, BLUE, MINRGB);					
					}
					else {
						blue_edge_detection.setPixel(i, j, RED, MAXRGB);
						blue_edge_detection.setPixel(i, j, GREEN, MAXRGB);
						blue_edge_detection.setPixel(i, j, BLUE, MAXRGB);	
					}

					// thresholding the combination of the three channels
					if (
						red_gradient_amplitude < T &&
						green_gradient_amplitude < T &&
						blue_gradient_amplitude < T
					) {
						tgt.setPixel(i, j, RED, MINRGB);
						tgt.setPixel(i, j, GREEN, MINRGB);
						tgt.setPixel(i, j, BLUE, MINRGB);
					}
					else {
						tgt.setPixel(i, j, RED, MAXRGB);
						tgt.setPixel(i, j, GREEN, MAXRGB);
						tgt.setPixel(i, j, BLUE, MAXRGB);
					}
				}
				else {
					tgt.setPixel(i, j, RED, checkValue(temp_img.getPixel(i, j, RED)));
					tgt.setPixel(i, j, GREEN, checkValue(temp_img.getPixel(i, j, GREEN)));
					tgt.setPixel(i, j, BLUE, checkValue(temp_img.getPixel(i, j, BLUE)));
					red_edge_detection.setPixel(i, j, RED, checkValue(temp_img_red.getPixel(i, j, RED)));
					red_edge_detection.setPixel(i, j, GREEN, checkValue(temp_img_red.getPixel(i, j, GREEN)));
					red_edge_detection.setPixel(i, j, BLUE, checkValue(temp_img_red.getPixel(i, j, BLUE)));
					green_edge_detection.setPixel(i, j, RED, checkValue(temp_img_green.getPixel(i, j, RED)));
					green_edge_detection.setPixel(i, j, GREEN, checkValue(temp_img_green.getPixel(i, j, GREEN)));
					green_edge_detection.setPixel(i, j, BLUE, checkValue(temp_img_green.getPixel(i, j, BLUE)));
					blue_edge_detection.setPixel(i, j, RED, checkValue(temp_img_blue.getPixel(i, j, RED)));
					blue_edge_detection.setPixel(i, j, GREEN, checkValue(temp_img_blue.getPixel(i, j, GREEN)));
					blue_edge_detection.setPixel(i, j, BLUE, checkValue(temp_img_blue.getPixel(i, j, BLUE)));
				}
			}
		}
		temp_img.copyImage(tgt);
		temp_img_red.copyImage(red_edge_detection);
		temp_img_green.copyImage(green_edge_detection);
		temp_img_blue.copyImage(blue_edge_detection);
	}

	char red_img_name[100] = "red_ed_";
	red_edge_detection.save(strcat(red_img_name, outfile));

	char green_img_name[100] = "green_ed_";
	green_edge_detection.save(strcat(green_img_name, outfile));

	char blue_img_name[100] = "blue_ed_";
	blue_edge_detection.save(strcat(blue_img_name, outfile));
}

/*-----------------------------------------------------------------------**/
HSI RGBtoHSI(int r, int g, int b) {
	double h, s, i;
	double norm_r = (double)r / (double)(r + g + b);
	double norm_g = (double)g / (double)(r + g + b);
	double norm_b = (double)b / (double)(r + g + b);

	double num = 0.5 * ((norm_r - norm_g) + (norm_r - norm_b));
	double den = sqrt(pow(norm_r - norm_g, 2) + ((norm_r - norm_b) * (norm_g - norm_b)));

	if (b <= g) {
		h = acos(num/den);
	}
	else {
		h = ((2 * PI) - (acos(num/den)));
	}

	s = 1 - (3 * min(min(norm_r, norm_g), norm_b));
	i = (double)(r + g + b) / (double)(3 * 255);

	HSI pix;
	pix.h = h;
	pix.i = i;
	pix.s = s;
	return pix;
}

RGB HSItoRGB(HSI pix) {
	double h, s, i;
	int r, g, b;

	h = pix.h;
	s = pix.s;
	i = pix.i;

	double x = i * (1 - s);

	if (h < ((2 * PI) / 3)) {
		RGB rgb_pix;
		double y = i * (1 + (s * cos(h) / cos((PI/3) - h)));
		double z = (3 * i) - (x + y);
		rgb_pix.r = y * 255;
		rgb_pix.g = z * 255;
		rgb_pix.b = x * 255;
		return rgb_pix;
	}
	else if (h < ((4 * PI) / 3)) {
		auto new_h = h - ((2 * PI) / 3);
		double y = i * (1 + (s * cos(new_h) / cos((PI/3) - new_h)));
		double z = (3 * i) - (x + y);
		RGB rgb_pix;
		rgb_pix.r = x * 255;
		rgb_pix.g = y * 255;
		rgb_pix.b = z * 255;
		return rgb_pix;
	}
	else  {
		auto new_h = h - ((4 * PI) / 3);
		double y = i * (1 + (s * cos(new_h) / cos((PI/3) - new_h)));
		double z = (3 * i) - (x + y);
		RGB rgb_pix;
		rgb_pix.r = z * 255;
		rgb_pix.g = x * 255;
		rgb_pix.b = y * 255;
		return rgb_pix;
	}
}

double calculateIValue(image& src, int i, int j, const roi& reg) {
	if (
		i >= reg.y && 
		i < (reg.y + reg.sy) &&
		j >= reg.x &&
		j < (reg.x + reg.sx)
	) {	// In the ROI
		int rgb_sum = (
			src.getPixel(i, j, RED) +
			src.getPixel(i, j, GREEN) +
			src.getPixel(i, j, BLUE)
		);

		double i_val = (double)rgb_sum / (double)(3 * 255);
		
		return i_val;
	}
	else {
		return 0;
	}
}

void utility::HSIEdgeDetection(image& src, image& tgt, const vector<roi>& regions, char* outfile) {
	tgt.resize(src.getNumberOfRows(), src.getNumberOfColumns());
	
	image grad_amplitude_img;
	grad_amplitude_img.resize(src.getNumberOfRows(), src.getNumberOfColumns());

	image temp_img, temp_img_grad_amp;

	temp_img.copyImage(src);
	temp_img_grad_amp.copyImage(src);
	for (int r = 0; r < regions.size(); r++) {
		int x = regions.at(r).x;
		int y = regions.at(r).y;
		int sx = regions.at(r).sx;
		int sy = regions.at(r).sy;
		int T = regions.at(r).color_threshold;

		for (int i = 0; i < temp_img.getNumberOfRows(); i++) {
			for (int j = 0; j < temp_img.getNumberOfColumns(); j++) {
				if (
					i >= y && 
					i < (y + sy) &&
					j >= x &&
					j < (x + sx)
				) { // inside the region
					HSI hsi_pixel = RGBtoHSI(
						temp_img.getPixel(i, j, RED),
						temp_img.getPixel(i, j, GREEN),
						temp_img.getPixel(i, j, BLUE)
					);

					double gx = 0, gy = 0;

					gx = (
						calculateIValue(temp_img, i + 1, j - 1, regions.at(r)) +
						calculateIValue(temp_img, i + 1, j, regions.at(r)) * 2 +
						calculateIValue(temp_img, i + 1, j + 1, regions.at(r)) -
						calculateIValue(temp_img, i - 1, j - 1, regions.at(r)) -
						calculateIValue(temp_img, i - 1, j, regions.at(r)) * 2 -
						calculateIValue(temp_img, i - 1, j + 1, regions.at(r))
					);

					gy = (
						calculateIValue(temp_img, i + 1, j + 1, regions.at(r)) +
						calculateIValue(temp_img, i, j + 1, regions.at(r)) * 2 +
						calculateIValue(temp_img, i - 1, j + 1, regions.at(r)) -
						calculateIValue(temp_img, i - 1, j - 1, regions.at(r)) -
						calculateIValue(temp_img, i, j - 1, regions.at(r)) * 2 -
						calculateIValue(temp_img, i + 1, j - 1, regions.at(r)) 
					);

					gx /= 8;
					gy /= 8;
					double ga_val = sqrt(pow(gx, 2) + pow(gy, 2));

					// setting image to gradient amplitude intensity
					HSI new_hsi_pix;
					new_hsi_pix.h = hsi_pixel.h;
					new_hsi_pix.s = hsi_pixel.s;
					new_hsi_pix.i = ga_val;
					RGB new_rgb_pixel = HSItoRGB(new_hsi_pix);
					grad_amplitude_img.setPixel(i, j, RED, checkValue(new_rgb_pixel.r));
					grad_amplitude_img.setPixel(i, j, GREEN, checkValue(new_rgb_pixel.g));
					grad_amplitude_img.setPixel(i, j, BLUE, checkValue(new_rgb_pixel.b));

					// setting image to binarized values
					if (
						new_rgb_pixel.r < T &&
						new_rgb_pixel.g < T &&
						new_rgb_pixel.b < T
					) {
						tgt.setPixel(i, j, RED, MINRGB);
						tgt.setPixel(i, j, GREEN, MINRGB);
						tgt.setPixel(i, j, BLUE, MINRGB);
					}
					else {
						tgt.setPixel(i, j, RED, MAXRGB);
						tgt.setPixel(i, j, GREEN, MAXRGB);
						tgt.setPixel(i, j, BLUE, MAXRGB);
					}
				}
				else {
					tgt.setPixel(i, j, RED, temp_img.getPixel(i, j, RED));
					tgt.setPixel(i, j, GREEN, temp_img.getPixel(i, j, GREEN));
					tgt.setPixel(i, j, BLUE, temp_img.getPixel(i, j, BLUE));
					grad_amplitude_img.setPixel(i, j, RED, temp_img_grad_amp.getPixel(i, j, RED));
					grad_amplitude_img.setPixel(i, j, GREEN, temp_img_grad_amp.getPixel(i, j, GREEN));
					grad_amplitude_img.setPixel(i, j, BLUE, temp_img_grad_amp.getPixel(i, j, BLUE));
				}
			}
		}
		temp_img.copyImage(tgt);
		temp_img_grad_amp.copyImage(grad_amplitude_img);
	}

	char grad_amp_img_name[100] = "grad_ampltiude_";
	grad_amplitude_img.save(strcat(grad_amp_img_name, outfile));
}