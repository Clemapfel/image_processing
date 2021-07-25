//
// Created by clem on 25.07.21.
//

#include <color.hpp>
#include <vector.hpp>
#include <utility>

namespace todo
{
    Color::Color()
    {
        r = 0;
        g = 0;
        b = 0;
        a = 1;
    }

    Color::Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
    {
        r = red;
        g = green;
        b = blue;
        a = alpha;
    }

    Color::Color(RGB rgb)
    {
        r = rgb.r;
        g = rgb.g;
        b = rgb.b;
        a = rgb.a;
    }

    Color::Color(HSV hsv)
    {
        auto as_rgb = convert<HSV, RGB>(hsv);
        r = as_rgb.r;
        g = as_rgb.g;
        b = as_rgb.b;
        a = hsv.a;
    }

    Color::Color(GrayScale grayscale)
    {
        r = grayscale.v;
        g = grayscale.v;
        b = grayscale.v;
        a = grayscale.a;
    }

    RGB && Color::as_rgb()
    {
        return convert<Color, RGB>(*this);
    }

    HSV && Color::as_hsv()
    {
        return convert<Color, HSV>(*this);
    }

    GrayScale && Color::as_grayscale()
    {
        return convert<Color, GrayScale>(*this);
    }

    Color & Color::operator=(RGB rgb)
    {
        r = rgb.r;
        g = rgb.g;
        b = rgb.b;
        a = rgb.a;
        return *this;
    }

    Color & Color::operator=(HSV hsv)
    {
        auto as_rgb = convert<HSV, RGB>(hsv);
        r = as_rgb.r;
        g = as_rgb.g;
        b = as_rgb.b;
        a = hsv.a;
        return *this;
    }

    Color & Color::operator=(GrayScale grayscale)
    {
        r = grayscale.v;
        g = grayscale.v;
        b = grayscale.v;
        a = grayscale.a;
        return *this;
    }

    bool Color::operator==(const Color& other) const
    {
        return this->r == other.r and this->g == other.g and this->b == other.b and this->a == other.a;
    }

    bool Color::operator!=(const Color& other) const
    {
        return not (*this == other);
    }

    Color && Color::invert() const
    {
        Color out;
        out.r = 1 - r;
        out.g = 1 - g;
        out.b = 1 - b;
        out.a = a;
        return std::forward<Color &&>(out);
    }

    Color && mix(Color a, Color b, float weight)
    {
         weight = clamp(0.f, 1.f, weight);

         float af = 1 - weight;
         float bf = weight;

         Color out;
         out.r = a.r * af + b.r * bf;
         out.g = a.g * af + b.g * bf;
         out.b = a.b * af + b.g * bf;
         out.a = a.a * af + b.a * bf;

         return std::forward<Color &&>(out);
    }
}

