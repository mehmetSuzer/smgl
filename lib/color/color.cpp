
#include "color.h"

Color::Color() : red(0), green(0), blue(0) {}

Color::Color(uint8_t red_, uint8_t green_, uint8_t blue_) : red(red_), green(green_), blue(blue_) {}

Color Color::operator + (const Color& other) const {
    uint16_t resultRed = red + other.red;
    uint16_t resultGreen = green + other.green;
    uint16_t resultBlue = blue + other.blue;
    
    if (resultRed > 255) {
        resultRed = 255;
    }
    if (resultGreen > 255) {
        resultGreen = 255;
    }
    if (resultBlue > 255) {
        resultBlue = 255;
    }

    return Color(resultRed, resultGreen, resultBlue);
}

void Color::operator += (const Color& other) {
    const uint16_t resultRed = red + other.red;
    const uint16_t resultGreen = green + other.green;
    const uint16_t resultBlue = blue + other.blue;

    red = (resultRed < 255) ? resultRed : 255;
    green = (resultGreen < 255) ? resultGreen : 255;
    blue = (resultBlue < 255) ? resultBlue : 255;
}

Color Color::operator * (float scalar) const {
    assert(scalar >= 0.0f);
    uint16_t resultRed = static_cast<uint16_t>(red * scalar);
    uint16_t resultGreen = static_cast<uint16_t>(green * scalar);
    uint16_t resultBlue = static_cast<uint16_t>(blue * scalar);

    if (resultRed > 255) {
        resultRed = 255;
    }
    if (resultGreen > 255) {
        resultGreen = 255;
    }
    if (resultBlue > 255) {
        resultBlue = 255;
    }

    return Color(resultRed, resultGreen, resultBlue);
}

void Color::operator *= (float scalar) {
    assert(scalar >= 0.0f);
    const uint16_t resultRed = static_cast<uint16_t>(red * scalar);
    const uint16_t resultGreen = static_cast<uint16_t>(green * scalar);
    const uint16_t resultBlue = static_cast<uint16_t>(blue * scalar);

    red = (resultRed < 255) ? resultRed : 255;
    green = (resultGreen < 255) ? resultGreen : 255;
    blue = (resultBlue < 255) ? resultBlue : 255;
}

Color Color::operator / (float scalar) const {
    assert(scalar > 0.0f);
    const uint8_t resultRed = static_cast<uint8_t>(red / scalar);
    const uint8_t resultGreen = static_cast<uint8_t>(green / scalar);
    const uint8_t resultBlue = static_cast<uint8_t>(blue / scalar);

    return Color(resultRed, resultGreen, resultBlue);
}
    
void Color::operator /= (float scalar) {
    assert(scalar > 0.0f);
    red = static_cast<uint8_t>(red / scalar);
    green = static_cast<uint8_t>(green / scalar);
    blue = static_cast<uint8_t>(blue / scalar);
}

Color Color::operator * (const Color& other) const {
    const uint8_t resultRed = static_cast<uint8_t>(red / 255.0f * other.red);
    const uint8_t resultGreen = static_cast<uint8_t>(green / 255.0f * other.green);
    const uint8_t resultBlue = static_cast<uint8_t>(blue / 255.0f * other.blue);

    return Color(resultRed, resultGreen, resultBlue);
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
const Color Color::DarkGray = Color(64, 64, 64);
const Color Color::Gray = Color(128, 128, 128); 
const Color Color::LightGray = Color(192, 192, 192);
const Color Color::White = Color(255, 255, 255);
