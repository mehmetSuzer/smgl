
#include "color.h"

Color::Color() : red(0), green(0), blue(0) {}

Color::Color(uint8_t red_, uint8_t green_, uint8_t blue_) : red(red_), green(green_), blue(blue_) {}

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

    red = (result_red <= 255) ? result_red : 255;
    green = (result_green <= 255) ? result_green : 255;
    blue = (result_blue <= 255) ? result_blue : 255;
}

Color Color::operator * (float scalar) const {
    assert(scalar >= 0.0f);
    uint16_t result_red = static_cast<uint16_t>(red*scalar);
    uint16_t result_green = static_cast<uint16_t>(green*scalar);
    uint16_t result_blue = static_cast<uint16_t>(blue*scalar);

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

void Color::operator *= (float scalar) {
    assert(scalar >= 0.0f);
    uint16_t result_red = static_cast<uint16_t>(red*scalar);
    uint16_t result_green = static_cast<uint16_t>(green*scalar);
    uint16_t result_blue = static_cast<uint16_t>(blue*scalar);

    red = (result_red <= 255) ? result_red : 255;
    green = (result_green <= 255) ? result_green : 255;
    blue = (result_blue <= 255) ? result_blue : 255;
}

Color Color::operator * (const Color& other) const {
    uint8_t result_red = static_cast<uint8_t>(red / 255.0f * other.red);
    uint8_t result_green = static_cast<uint8_t>(green / 255.0f * other.green);
    uint8_t result_blue = static_cast<uint8_t>(blue / 255.0f * other.blue);

    return Color(result_red, result_green, result_blue);
}

void Color::operator *= (const Color& other) {
    red = static_cast<uint8_t>(red / 255.0f * other.red);
    green = static_cast<uint8_t>(green / 255.0f * other.green);
    blue = static_cast<uint8_t>(blue / 255.0f * other.blue);
}

const Color Color::Red = Color(255, 0, 0);
const Color Color::Green = Color(0, 255, 0);
const Color Color::Blue = Color(0, 0, 255);

const Color Color::Yellow = Color(255, 255, 0);
const Color Color::Cyan = Color(0, 255, 255);
const Color Color::Magenta = Color(255, 0, 255);

const Color Color::Black = Color(0, 0, 0);
const Color Color::LightGray = Color(64, 64, 64);
const Color Color::Gray = Color(128, 128, 128);
const Color Color::DarkGray = Color(192, 192, 192);
const Color Color::White = Color(255, 255, 255);
