U09026779, Andrew Fernandez

Parameters
--------------------------
There is one file that is read by the program. It has very strict options for each 
line in the file. It includes the information for each ROI, parameters for the 
various functions for each ROI, and the images the appropriate functions should be run on. 
There are cycles for the lines. For every image/function that you want to have operated on,
the first line will contain the name of the source image, what you wish to name the target image,
the function that you want to operate over the image on, and the number of ROIs (n) used for the
function. The following n lines will be information given for the parameters for each ROI for each 
function. Each of these n lines will contain the following information in the following order: x, y, sx, sy, 
the threshold, and the direction. Everything is separated by spaces. The first lines will be in this form: 
input_image output_image function. Where the function can be gray_edge (for gray edge detection), 
rgb_edge (for RGB color edge detection), hsi_edge (for HSI color edge detection). A side-note is that the 
parameters file should be in the following path: /Project2/project/bin/. The following image is an example of 
a properly constructed parameters file called parameters.txt:
baboon.pgm baboon_edge_d.pgm gray_edge 2
0 0 100 300 10 45
320 80 150 150 15 100
wheel.pgm wheel_d.pgm gray_edge 1
0 0 300 300 10 45
ball.ppm ball_edge_d.ppm rgb_edge 2
100 50 100 100  10 45
210 200 280 215 10 45
wine.ppm wine_edge_d.ppm rgb_edge 3
0 0 100 300 10 45
200 600 250 100 10 45
400 400 100 100 10 45
ball.ppm ball_hsi_edge_d.ppm hsi_edge 2
100 50 100 100  10 45
210 200 280 215 10 45
wine.ppm wine_hsi_edge_d.ppm hsi_edge 3
0 0 100 300 10 45
200 600 250 100 10 45
400 400 100 100 10 45


Compilation and Execution
--------------------------
In order to compile this program, make needs to be run twice. Once inside the Project2 directory, 
run make inside of /Project2/iptools to compile both image.cpp and utility.cpp. Then travel to 
/Project2/project and run make to compile iptool.cpp. This will create the executable in 
/Project2/project/bin of the name iptool. To run the program, travel to /Project2/project/bin/ 
and execute ./iptool <file_name> where <file_name> is the name of your parameters file located 
inside /Project2/project/bin/.