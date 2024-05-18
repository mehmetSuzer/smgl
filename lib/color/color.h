
#ifndef __COLOR_H__
#define __COLOR_H__

#include <stdint.h>
#include <string.h>
#include <math.h>

class Color {
public:
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    Color();
    Color(uint8_t red_, uint8_t green_, uint8_t blue_);
    Color(const char* color);

    Color operator + (const Color& other) const;
    void operator += (const Color& other);

    Color operator * (float scalar) const;
    void operator *= (float scalar);

    Color operator * (const Color& other) const;
    void operator *= (const Color& other);
};

#endif // __COLOR_H__
