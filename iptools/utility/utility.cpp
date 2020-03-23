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
int getMaxIntensity(image img, roi region) {
	int max_val = 0;
	for (int i = region.y; i < region.sy; i++) {
		for (int j = region.x; j < region.sx; j++) {
			if (img.getPixel(i, j) > max_val) {
				max_val = img.getPixel(i, j);
			}
		}
	}

	return max_val;
}

int getMinIntensity(image img, roi region) {
	int min_val = 255;
	for (int i = region.y; i < region.sy; i++) {
		for (int j = region.x; j < region.sx; j++) {
			if (img.getPixel(i, j) < min_val) {
				min_val = img.getPixel(i, j);
			}
		}
	}

	return min_val;
}

image makeHistogram(vector<int> intensity_counts) {
	image hist;
	hist.resize(256, 256);

	// getting max count
	int max_count = 0;
	for (int i = 0; i < intensity_counts.size(); i++) {
		if (intensity_counts.at(i) > max_count) {
			max_count = intensity_counts.at(i);
		}
	}
	
	double countForPixel = (double)max_count / 256;

	for (int i = 0; i < intensity_counts.size(); i++) {
		intensity_counts.at(i) /= countForPixel;
		for (int c = 0; c < intensity_counts.at(i); c++) {
			hist.setPixel(256 - c, i, MAXRGB);
		}
	}

	// cout << "returning hist " << endl;
	return hist;
}

void utility::histStretch(image& src, image& tgt, const vector<roi>& regions, string file_name) {
	tgt.resize(src.getNumberOfRows(), src.getNumberOfColumns());
	image temp_img;
	temp_img.copyImage(src);
	for (int r = 0; r < regions.size(); r++) {
		// cout << "in r loop" << endl;
		image original_hist, new_img_hist;
		original_hist.resize(256, 256);
		new_img_hist.resize(256, 256);
		vector<int> original_hist_vec(256, 0);
		vector<int> new_img_hist_vec(256, 0);
		int x = regions.at(r).x;
		int y = regions.at(r).y;
		int sx = regions.at(r).sx;
		int sy = regions.at(r).sy;
		int a = regions.at(r).a;
		int b = regions.at(r).b;

		for (int i = 0; i < temp_img.getNumberOfRows(); i++) {
			for (int j = 0; j < temp_img.getNumberOfColumns(); j++) {
				if (
					i >= y &&
					i < (y + sy) &&
					j >= x &&
					j < (x + sx)
				) {
					int intensity_value = temp_img.getPixel(i, j);
					original_hist_vec.at(intensity_value) += 1;

					int curr_pixel = temp_img.getPixel(i, j);
					if (curr_pixel < a) {
						tgt.setPixel(i, j, MINRGB); // MAYBE CHECK I J VALUES IF THIS DOESNT WORK
					}
					else if (curr_pixel > b) {
						tgt.setPixel(i, j, MAXRGB); // MAYBE CHECK I J VALUES IF THIS DOESNT WORK
					}
					else {
						int newVal = (curr_pixel - a) * (255/(b - a));
						tgt.setPixel(i, j, checkValue(newVal));
					}
				}
				else {
					tgt.setPixel(i, j, checkValue(temp_img.getPixel(i, j)));
				}			
			}
		}

		temp_img.copyImage(tgt);

		// creating new image histogram
		for (int i = 0; i < tgt.getNumberOfRows(); i++) {
			for (int j = 0; j < tgt.getNumberOfColumns(); j++) {
				if (
					i >= y &&
					i < (y + sy) &&
					j >= x &&
					j < (x + sx)
				) {
					int intensity_value = tgt.getPixel(i, j);
					new_img_hist_vec.at(intensity_value) += 1;
				}		
			}
		}

		original_hist = makeHistogram(original_hist_vec);
		if (original_hist.save(("orignal_hist_" + file_name + "_roi" + to_string(r)).c_str())) {
			cout << "saved original hist" << endl;
		}
		else {
			cout << "failed to save original hist" << endl;
		}

		new_img_hist = makeHistogram(new_img_hist_vec);
		if (new_img_hist.save(("new_img_hist_" + file_name + "_roi" + to_string(r)).c_str())) {
			cout << "saved new_img_hist" << endl;
		}
		else {
			cout << "failed to save new_img_hist" << endl;
		}
	}
}

double getAvgVec(vector<int> nums) {
	int sum = 0;
	for (int x : nums) {
		sum += x;
	}

	return sum/nums.size();
}

void utility::optimalThresholding(image& src, image& tgt, const vector<roi>& regions) {
	tgt.resize(src.getNumberOfRows(), src.getNumberOfColumns());
	image temp_img;
	temp_img.copyImage(src);
	for (int r = 0; r < regions.size(); r++) {
		int x = regions.at(r).x;
		int y = regions.at(r).y;
		int sx = regions.at(r).sx;
		int sy = regions.at(r).sy;
		vector<int> roi_pixel_values;

		for (int i = 0; i < temp_img.getNumberOfRows(); i++) {
			for (int j = 0; j < temp_img.getNumberOfColumns(); j++) {
				if (
					i >= y &&
					i < (y + sy) &&
					j >= x &&
					j < (x + sx)
				) {
					roi_pixel_values.push_back(temp_img.getPixel(i, j));
				}				
			}
		}

		double init_thresh = getAvgVec(roi_pixel_values);
		recursiveOptThresh(temp_img, tgt, regions.at(r), roi_pixel_values, init_thresh);
		temp_img.copyImage(tgt);
	}
}

void utility::recursiveOptThresh(image& src, image& tgt, const roi& reg, const vector<int>& pixelVals, const double& thresh) {
	vector<int> foreground, background;
	for (int p : pixelVals) {
		if (p <= thresh) {
			background.push_back(p);
		}
		else { // p > thresh
			foreground.push_back(p);
		}
	}

	double new_thresh = ((getAvgVec(background) + getAvgVec(foreground)) / 2);

	if (abs(new_thresh - thresh) >= THRESH_LIMIT) {
		recursiveOptThresh(src, tgt, reg, pixelVals, new_thresh);
	}
	else {
		for (int i = 0; i < src.getNumberOfRows(); i++) {
			for (int j = 0; j < src.getNumberOfColumns(); j++) {
				if (
					i >= reg.y &&
					i < (reg.y + reg.sy) &&
					j >= reg.x &&
					j < (reg.x + reg.sx)
				) {
					if (src.getPixel(i, j) < new_thresh) {
						tgt.setPixel(i, j, MINRGB);
					}
					else {
						tgt.setPixel(i, j, MAXRGB);
					}
				}
				else {
					tgt.setPixel(i, j, checkValue(src.getPixel(i, j)));
				}
			}
		}
	}
}

double recuriveHistOptThresh(const roi& reg, const vector<int>& pixelVals, const double& thresh) {
	vector<int> foreground, background;
	for (int p : pixelVals) {
		if (p <= thresh) {
			background.push_back(p);
		}
		else { // p > thresh
			foreground.push_back(p);
		}
	}

	double new_thresh = ((getAvgVec(background) + getAvgVec(foreground)) / 2);

	if (abs(new_thresh - thresh) >= THRESH_LIMIT) {
		return recuriveHistOptThresh(reg, pixelVals, new_thresh);
	}
	else {
		return new_thresh;
	}	
}

void utility::combHistOptThresh(image& src, image& tgt, const vector<roi>& regions, string file_name) {
	tgt.resize(src.getNumberOfRows(), src.getNumberOfColumns());
	image temp_img;
	temp_img.copyImage(src);
	for (int r = 0; r < regions.size(); r++) {
		int x = regions.at(r).x;
		int y = regions.at(r).y;
		int sx = regions.at(r).sx;
		int sy = regions.at(r).sy;
		vector<int> roi_pixel_values;

		for (int i = 0; i < temp_img.getNumberOfRows(); i++) {
			for (int j = 0; j < temp_img.getNumberOfColumns(); j++) {
				if (
					i >= y &&
					i < (y + sy) &&
					j >= x &&
					j < (x + sx)
				) {
					roi_pixel_values.push_back(temp_img.getPixel(i, j));
				}				
			}
		}	

		// thresholding
		double init_thresh = getAvgVec(roi_pixel_values);
		double final_thresh = recuriveHistOptThresh(regions.at(r), roi_pixel_values, init_thresh);
		vector<int> roi_foreground, roi_background;

		for (int p : roi_pixel_values) {
			if (p <= final_thresh) {
				roi_background.push_back(p);
			}
			else {
				roi_foreground.push_back(p);
			}
		}
			
		// GETTING MIN AND MAX OF FOREGROUND AND BACKGROUND
		auto background_min_iter = min_element(roi_background.begin(), roi_background.end());
		int background_min = *background_min_iter;
		auto background_max_iter = max_element(roi_background.begin(), roi_background.end());
		int background_max = *background_max_iter;
		auto foreground_min_iter = min_element(roi_foreground.begin(), roi_foreground.end());
		int foreground_min = *foreground_min_iter;
		auto foreground_max_iter = max_element(roi_foreground.begin(), roi_foreground.end());
		int foreground_max = *foreground_max_iter;

		// histogram stretching
		for (int i = 0; i < temp_img.getNumberOfRows(); i++) {
			for (int j = 0; j < temp_img.getNumberOfColumns(); j++) {
				if (
					i >= y &&
					i < (y + sy) &&
					j >= x &&
					j < (x + sx)
				) {
					if (temp_img.getPixel(i, j) <= final_thresh) { // in the background
						int curr_pixel = temp_img.getPixel(i, j);
						if (curr_pixel < background_min) { 
							tgt.setPixel(i, j, MINRGB); // MAYBE CHECK I J VALUES IF THIS DOESNT WORK
						}
						else if (curr_pixel > background_max) {
							tgt.setPixel(i, j, MAXRGB); // MAYBE CHECK I J VALUES IF THIS DOESNT WORK
						}
						else {
							int newVal = (curr_pixel - background_min) * (255/(background_max - background_min));
							tgt.setPixel(i, j, checkValue(newVal));
						}					
					}
					else { /// in the foreground
						int curr_pixel = temp_img.getPixel(i, j);
						if (curr_pixel < foreground_min) {
							tgt.setPixel(i, j, MINRGB); // MAYBE CHECK I J VALUES IF THIS DOESNT WORK
						}
						else if (curr_pixel > foreground_max) {
							tgt.setPixel(i, j, MAXRGB); // MAYBE CHECK I J VALUES IF THIS DOESNT WORK
						}
						else {
							int newVal = (curr_pixel - foreground_min) * (255/(foreground_max - foreground_min));
							tgt.setPixel(i, j, checkValue(newVal));
						}	
					}
				}
				else {
					tgt.setPixel(i, j, checkValue(temp_img.getPixel(i, j)));
				}			
			}
		}

		temp_img.copyImage(tgt);
	}
}

void utility::histRed(image& src, image& tgt, const vector<roi>& regions, string file_name) {
	tgt.resize(src.getNumberOfRows(), src.getNumberOfColumns());
	image temp_img;
	temp_img.copyImage(src);

	for (int r = 0; r < regions.size(); r++) {
		int x = regions.at(r).x;
		int y = regions.at(r).y;
		int sx = regions.at(r).sx;
		int sy = regions.at(r).sy;
		int a = regions.at(r).red_a;
		int b = regions.at(r).red_b;

		for (int i = 0; i < temp_img.getNumberOfRows(); i++) {
			for (int j = 0; j < temp_img.getNumberOfColumns(); j++) {
				if (
					i >= y &&
					i < (y + sy) &&
					j >= x &&
					j < (x + sx)
				) {
					int curr_pixel = temp_img.getPixel(i, j, RED);
					if (curr_pixel < a) {
						tgt.setPixel(i, j, RED, MINRGB);
					}
					else if (curr_pixel > b) {
						tgt.setPixel(i, j, RED, MAXRGB);
					}
					else {
						int newVal = (curr_pixel - a) * (255/(b - a));
						tgt.setPixel(i, j, RED, checkValue(newVal));
					}
				}
				else {
					tgt.setPixel(i, j, RED, checkValue(temp_img.getPixel(i, j, RED)));
				}
				tgt.setPixel(i, j, GREEN, checkValue(temp_img.getPixel(i, j, GREEN)));
				tgt.setPixel(i, j, BLUE, checkValue(temp_img.getPixel(i, j, BLUE)));
			}
		}

		temp_img.copyImage(tgt);				
	}
}

void utility::histGreen(image& src, image& tgt, const vector<roi>& regions, string file_name) {
	tgt.resize(src.getNumberOfRows(), src.getNumberOfColumns());
	image temp_img;
	temp_img.copyImage(src);

	for (int r = 0; r < regions.size(); r++) {
		int x = regions.at(r).x;
		int y = regions.at(r).y;
		int sx = regions.at(r).sx;
		int sy = regions.at(r).sy;
		int a = regions.at(r).green_a;
		int b = regions.at(r).green_b;

		for (int i = 0; i < temp_img.getNumberOfRows(); i++) {
			for (int j = 0; j < temp_img.getNumberOfColumns(); j++) {
				if (
					i >= y &&
					i < (y + sy) &&
					j >= x &&
					j < (x + sx)
				) {
					int curr_pixel = temp_img.getPixel(i, j, GREEN);
					if (curr_pixel < a) {
						tgt.setPixel(i, j, GREEN, MINRGB);
					}
					else if (curr_pixel > b) {
						tgt.setPixel(i, j, GREEN, MAXRGB);
					}
					else {
						int newVal = (curr_pixel - a) * (255/(b - a));
						tgt.setPixel(i, j, GREEN, checkValue(newVal));
					}
				}
				else {
					tgt.setPixel(i, j, GREEN, checkValue(temp_img.getPixel(i, j, GREEN)));
				}
				tgt.setPixel(i, j, RED, checkValue(temp_img.getPixel(i, j, RED)));
				tgt.setPixel(i, j, BLUE, checkValue(temp_img.getPixel(i, j, BLUE)));
			}
		}

		temp_img.copyImage(tgt);				
	}
}

void utility::histBlue(image& src, image& tgt, const vector<roi>& regions, string file_name) {
	tgt.resize(src.getNumberOfRows(), src.getNumberOfColumns());
	image temp_img;
	temp_img.copyImage(src);

	for (int r = 0; r < regions.size(); r++) {
		int x = regions.at(r).x;
		int y = regions.at(r).y;
		int sx = regions.at(r).sx;
		int sy = regions.at(r).sy;
		int a = regions.at(r).blue_a;
		int b = regions.at(r).blue_b;

		for (int i = 0; i < temp_img.getNumberOfRows(); i++) {
			for (int j = 0; j < temp_img.getNumberOfColumns(); j++) {
				if (
					i >= y &&
					i < (y + sy) &&
					j >= x &&
					j < (x + sx)
				) {
					int curr_pixel = temp_img.getPixel(i, j, BLUE);
					if (curr_pixel < a) {
						tgt.setPixel(i, j, BLUE, MINRGB);
					}
					else if (curr_pixel > b) {
						tgt.setPixel(i, j, BLUE, MAXRGB);
					}
					else {
						int newVal = (curr_pixel - a) * (255/(b - a));
						tgt.setPixel(i, j, BLUE, checkValue(newVal));
					}
				}
				else {
					tgt.setPixel(i, j, BLUE, checkValue(temp_img.getPixel(i, j, BLUE)));
				}
				tgt.setPixel(i, j, RED, checkValue(temp_img.getPixel(i, j, RED)));
				tgt.setPixel(i, j, GREEN, checkValue(temp_img.getPixel(i, j, GREEN)));
			}
		}

		temp_img.copyImage(tgt);				
	}
}

void utility::histRGB(image& src, image& tgt, const vector<roi>& regions, string file_name) {
	tgt.resize(src.getNumberOfRows(), src.getNumberOfColumns());
	image temp_img;
	temp_img.copyImage(src);

	for (int r = 0; r < regions.size(); r++) {
		int x = regions.at(r).x;
		int y = regions.at(r).y;
		int sx = regions.at(r).sx;
		int sy = regions.at(r).sy;
		int a = regions.at(r).rgb_a;
		int b = regions.at(r).rgb_b;
		for (int i = 0; i < temp_img.getNumberOfRows(); i++) {
			for (int j = 0; j < temp_img.getNumberOfColumns(); j++) {
				if (
					i >= y &&
					i < (y + sy) &&
					j >= x &&
					j < (x + sx)
				) {
					// RED
					int curr_red_pixel = temp_img.getPixel(i, j, RED);
					if (curr_red_pixel < a) {
						tgt.setPixel(i, j, RED, MINRGB);
					}
					else if (curr_red_pixel > b) {
						tgt.setPixel(i, j, RED, MAXRGB);
					}
					else {
						int newVal = (curr_red_pixel - a) * (255/(b - a));
						tgt.setPixel(i, j, RED, checkValue(newVal));
					}

					// GREEN
					int curr_green_pixel = temp_img.getPixel(i, j, GREEN);
					if (curr_green_pixel < a) {
						tgt.setPixel(i, j, GREEN, MINRGB);
					}
					else if (curr_green_pixel > b) {
						tgt.setPixel(i, j, GREEN, MAXRGB);
					}
					else {
						int newVal = (curr_green_pixel - a) * (255/(b - a));
						tgt.setPixel(i, j, GREEN, checkValue(newVal));
					}

					// BLUE
					int curr_blue_pixel = temp_img.getPixel(i, j, BLUE);
					if (curr_blue_pixel < a) {
						tgt.setPixel(i, j, BLUE, MINRGB);
					}
					else if (curr_blue_pixel > b) {
						tgt.setPixel(i, j, BLUE, MAXRGB);
					}
					else {
						int newVal = (curr_blue_pixel - a) * (255/(b - a));
						tgt.setPixel(i, j, BLUE, checkValue(newVal));
					}
				}
				else {
					tgt.setPixel(i, j, RED, checkValue(temp_img.getPixel(i, j, RED)));
					tgt.setPixel(i, j, GREEN, checkValue(temp_img.getPixel(i, j, GREEN)));
					tgt.setPixel(i, j, BLUE, checkValue(temp_img.getPixel(i, j, BLUE)));
				}
			}
		}
		
		temp_img.copyImage(tgt);
	}
}