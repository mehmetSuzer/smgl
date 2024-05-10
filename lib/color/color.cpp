
#include "color.h"


Color::Color() : red(0), green(0), blue(0) {}

Color::Color(uint8_t red, uint8_t green, uint8_t blue) {
    Color::red = red;
    Color::green = green;
    Color::blue = blue;
}

Color::Color(const char* color) {
    if (strcmp(color, "RED") == 0) {
        red = 255;
        green = 0;
        blue = 0;
    } else if (strcmp(color, "GREEN") == 0) {
        red = 0;
        green = 255;
        blue = 0;
    } else if (strcmp(color, "BLUE") == 0) {
        red = 0;
        green = 0;
        blue = 255;
    } else if (strcmp(color, "YELLOW") == 0) {
        red = 255;
        green = 255;
        blue = 0;
    } else if (strcmp(color, "MAGENTA") == 0) {
        red = 255;
        green = 0;
        blue = 255;
    } else if (strcmp(color, "CYAN") == 0) {
        red = 0;
        green = 255;
        blue = 255;
    } else if (strcmp(color, "WHITE") == 0) {
        red = 255;
        green = 255;
        blue = 255;
    } else if (strcmp(color, "GRAY") == 0) {
        red = 128;
        green = 128;
        blue = 128;
    } else if (strcmp(color, "LIGHT GRAY") == 0) {
        red = 192;
        green = 192;
        blue = 192;
    } else if (strcmp(color, "DARK GRAY") == 0) {
        red = 64;
        green = 64;
        blue = 64;
    } else {
        red = 0;
        green = 0;
        blue = 0;
    } 
}

Color Color::operator + (const Color& other) const {
    uint16_t result_red = red + other.red;
    uint16_t result_green = green + other.green;
    uint16_t result_blue = blue + other.blue;
    if (result_red > 255) {
        result_red = 255;
    }
    if (result_green > 255) {
        result_green = 255;
    }
    if (result_blue > 255) {
        result_blue = 255;
    }
    return Color(result_red, result_green, result_blue);
}

void Color::operator += (const Color& other) {
    uint16_t result_red = red + other.red;
    uint16_t result_green = green + other.green;
    uint16_t result_blue = blue + other.blue;
    if (result_red > 255) {
        result_red = 255;
    }
    if (result_green > 255) {
        result_green = 255;
    }
    if (result_blue > 255) {
        result_blue = 255;
    }
    red = result_red;
    green = result_green;
    blue = result_blue;
}

Color Color::operator * (float scalar) const {
    if (scalar < 0.0f) {
        scalar = 0.0f;
    } else if (scalar > 1.0f) {
        scalar = 1.0f;
    }
    return Color(red * scalar, green * scalar, blue * scalar);
}

void Color::operator *= (float scalar) {
    if (scalar < 0.0f) {
        scalar = 0.0f;
    } else if (scalar > 1.0f) {
        scalar = 1.0f;
    }
    red = (uint8_t)(red*scalar);
    green = (uint8_t)(green*scalar);
    blue = (uint8_t)(blue*scalar);
}

Color Color::operator * (const Color& other) const {
    uint8_t result_red = (uint8_t)(red / 255.0f * other.red);
    uint8_t result_green = (uint8_t)(green / 255.0f * other.green);
    uint8_t result_blue = (uint8_t)(blue / 255.0f * other.blue);
    return Color(result_red, result_green, result_blue);
}

void Color::operator *= (const Color& other) {
    red = (uint8_t)(red / 255.0f * other.red);
    green = (uint8_t)(green / 255.0f * other.green);
    blue = (uint8_t)(blue / 255.0f * other.blue);
}

Color Color::operator / (float scalar) const {
    if (scalar <= 0.0f) {
        scalar = INFINITY;
    }
    return Color(red / scalar, green / scalar, blue / scalar);
}

void Color::operator /= (float scalar) {
    if (scalar <= 0.0f) {
        scalar = INFINITY;
    }
    red = (uint8_t)((float)red/scalar);
    green = (uint8_t)((float)green/scalar);
    blue = (uint8_t)((float)blue/scalar);
}

void Color::print(void) const {
    std::cout << (uint32_t)red << " " << (uint32_t)green << " " << (uint32_t)blue << std::endl;
}