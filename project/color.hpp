#ifndef COLOR_HPP
#define COLOR_HPP

class color {
public:
    color() = default;
    ~color() = default;
    color(int red, int green, int blue) {
        R = red;
        G = green;
        B = blue;
    }

    int R;
    int G;
    int B;
};

#endif