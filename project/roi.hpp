#ifndef ROI_HPP
#define ROI_HPP

class roi 
{
public:
    roi() = default;
    roi(int x_i, int y_i, int sx_i, int sy_i, 
        int a_i, int b_i,
        int red_a_i, int red_b_i,
        int green_a_i, int green_b_i,
        int blue_a_i, int blue_b_i,
        int rgb_a_i, int rgb_b_i) {
            x = x_i;
            y = y_i;
            sx = sx_i;
            sy = sy_i;
            a = a_i;
            b = b_i;
            red_a = red_a_i;
            red_b = red_b_i;
            green_a = green_a_i;
            green_b = green_b_i;
            blue_a = blue_a_i;
            blue_b = blue_b_i;
            rgb_a = rgb_a_i;
            rgb_b = rgb_b_i;
        };
    ~roi() = default;
    
    int x;
    int y;
    int sx;
    int sy;
    int a, b;
    int red_a, red_b;
    int green_a, green_b;
    int blue_a, blue_b;
    int rgb_a, rgb_b;
};

#endif