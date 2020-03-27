#ifndef ROI_HPP
#define ROI_HPP

class roi 
{
public:
    roi() = default;
    roi(int x_i, int y_i, int sx_i, int sy_i, 
        int gray_threshold_i, int gray_direction_i,
        int color_threshold_i, int color_direction_i) {
            x = x_i;
            y = y_i;
            sx = sx_i;
            sy = sy_i;
            gray_threshold = gray_threshold_i;
            gray_direction = gray_direction_i;
            color_threshold = color_direction_i;
            color_direction = color_direction_i;
        };
    ~roi() = default;
    
    int x;
    int y;
    int sx;
    int sy;
    int gray_threshold;
    int gray_direction;
    int color_threshold;
    int color_direction;
};

#endif