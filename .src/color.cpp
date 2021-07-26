//
// Copyright 2021 Clemens Cords
// Created on 25.07.21 by clem (mail@clemens-cords.com)
//

#include <color.hpp>
#include <vector.hpp>
#include <utility>

namespace crisp
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
        auto as_rgb = convert_to<RGB>(hsv);
        r = as_rgb.r;
        g = as_rgb.g;
        b = as_rgb.b;
        a = hsv.a;
    }

    Color::Color(HSL hsl)
    {
        auto as_rgb = convert_to<RGB>(hsl);
        r = as_rgb.r;
        g = as_rgb.g;
        b = as_rgb.b;
        a = hsl.a;
    }

    Color::Color(GrayScale grayscale)
    {
        r = grayscale.v;
        g = grayscale.v;
        b = grayscale.v;
        a = grayscale.a;
    }

    RGB Color::as_rgb()
    {
        return convert_to<RGB>(*this);
    }

    HSV Color::as_hsv()
    {
        return convert_to<HSV>(*this);
    }

    HSL Color::as_hsl()
    {
        return convert_to<HSL>(*this);
    }

    Vector4f Color::as_float_vec()
    {
        auto rf = static_cast<float>(r / uint8_t(255));
        auto gf = static_cast<float>(g / uint8_t(255));
        auto bf = static_cast<float>(b / uint8_t(255));
        auto af = static_cast<float>(a / uint8_t(255));

        return Vector4f(rf, gf, bf, af);
    }

    GrayScale Color::as_grayscale()
    {
        return convert_to<GrayScale>(*this);
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
        auto as_rgb = convert_to<RGB>(hsv);
        r = as_rgb.r;
        g = as_rgb.g;
        b = as_rgb.b;
        a = hsv.a;
        return *this;
    }

    Color & Color::operator=(HSL hsl)
    {
        auto as_rgb = convert_to<RGB>(hsl);
        r = as_rgb.r;
        g = as_rgb.g;
        b = as_rgb.b;
        a = hsl.a;
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

    Color Color::invert() const
    {
        Color out;
        out.r = 1 - r;
        out.g = 1 - g;
        out.b = 1 - b;
        out.a = a;
        return out;
    }
}

