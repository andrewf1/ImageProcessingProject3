U09026779, Andrew Fernandez

Parameters
--------------------------
There is one file that is read by the program. It has very strict options for each 
line in the file. It includes the information for each ROI, parameters for the 
various functions for each ROI, and the images the appropriate functions should be run on. 
The first line is the number of ROIs (n) that the program will consider (maximum of 3). 
The following n lines will be information given for the parameters for each ROI for each function. 
Each of these n lines will contain the following information in the following order: x, y, sx, sy, 
a (for regular histogram stretching), b (for regular histogram stretching), 
a_red (for histogram stretching on red channel), b_red (for histogram stretching on red channel), 
a_green (for histogram stretching on green channel), b_green (for histogram stretching on green channel),
a_blue (for histogram stretching on blue channel), b_blue (for histogram stretching on blue channel), 
a_rgb (for histogram stretching on all 3 rgb channel together), and b_rgb (for histogram stretching on all 3 rgb channel together). 
They will not contain commas. After those n lines, the rest of the 
lines in the file should be images to run the specified functions on and how to name the output image. 
These lines will be in this form: input_image output_image function. Where the function can be 
hist_stretch (for Histogram Stretching), opt_thresh (for Optimal Thresholding), 
comb_hist_thresh (for Combining the previous 2 image analysis operations), 
hist_red (for Histogram Stretching on the red channel of an image), hist_green (for Histogram Stretching on the green channel of an image),
hist_blue (for Histogram Stretching on the blue channel of an image), 
and hist_rgb (for Histogram Stretching on all 3 rgb channels of an image). A side-note is that the parameters file 
must end with an empty newline. Another note is that the parameters file should be in the following path: 
/Project2/project/bin/. The following image is an example of a properly constructed parameters file 
called parameters.txt:
1
0 0 400 300 100 200 50 150 50 150 50 150 50 150
park.pgm park_hs.pgm hist_stretch
floor.pgm floor_ot.pgm opt_thresh
park.pgm park_comb_hist_thresh.pgm comb_hist_thresh
ball.ppm ball_hist_red.ppm hist_red
ball.ppm ball_hist_green.ppm hist_green
ball.ppm ball_hist_blue.ppm hist_blue
ball.ppm ball_hist_rgb.ppm hist_rgb


Compilation and Execution
--------------------------
In order to compile this program, make needs to be run twice. Once inside the Project2 directory, 
run make inside of /Project2/iptools to compile both image.cpp and utility.cpp. Then travel to 
/Project2/project and run make to compile iptool.cpp. This will create the executable in 
/Project2/project/bin of the name iptool. To run the program, travel to /Project2/project/bin/ 
and execute ./iptool <file_name> where <file_name> is the name of your parameters file located 
inside /Project2/project/bin/.
