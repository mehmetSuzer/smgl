
#ifndef __COLOR_H__
#define __COLOR_H__

#include <assert.h>
#include <stdint.h>

class Color {
public:
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    static const Color Red;
    static const Color Green;
    static const Color Blue;

    static const Color Yellow;
    static const Color Cyan;
    static const Color Magenta;

    static const Color Black;
    static const Color DarkGray;
    static const Color Gray;
    static const Color LightGray;
    static const Color White;

    Color();
    Color(uint8_t red_, uint8_t green_, uint8_t blue_);

    Color operator + (const Color& other) const;
    void operator += (const Color& other);

    Color operator * (float scalar) const;
    void operator *= (float scalar);

    Color operator / (float scalar) const;
    void operator /= (float scalar);

    Color operator * (const Color& other) const;
    void operator *= (const Color& other);
};

#endif // __COLOR_H__
